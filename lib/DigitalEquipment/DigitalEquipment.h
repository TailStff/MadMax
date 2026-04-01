#ifndef MADMAXDIGITALEQUIPMENT_H
#define MADMAXDIGITALEQUIPMENT_H

#include "DigitalEquipmentStatus.h"
#include "IPrimitive.h"
#include "FeedbackError.h"
#include "structFaults.h"
#include "RunTime.h"
#include "Accum.h"

namespace MadMax
{
    struct DigitalEquipementPersistencyValues
    {
        uint64_t runTime;
        uint64_t startCount;
    };

    class DigitalEquipment : public IPrimitiveTyped<DigitalEquipementPersistencyValues>, public IPrimitive
    {
    private:
        ExecutionEnv *executionEnv;
        FeedbackError *feedbackErrorObj;
        RunTime *runTimeObj;
        Accum<uint64_t> *accumObj;

        DigitalEquipmentStatus status;

        bool memAcknowledge;

    public:
        // Constructors
        DigitalEquipment(ExecutionEnv *_executionEnv, DigitalEquipementPersistencyValues data = {.runTime = 0UL, .startCount = 0UL});
        ~DigitalEquipment();

        bool Evaluate(bool cmd, bool feedback, bool fault, bool acknowledge, uint32_t feedbackDelay, DigitalEquipmentStatus *outStatus = nullptr);

        const DigitalEquipmentStatus &GetStatus() const;
        bool GetValue() const;

        void SetRuntime(uint64_t runtime);
        void SetStartCount(uint64_t startCount);

        /// @brief Static function that give the size of the serialized data of the object, here we just serialize runTime and startCount Values
        /// @return Size in bytes of the serialized data of the object
        static size_t GetSerializedSize()
        {
            return sizeof(DigitalEquipementPersistencyValues);
        }

        void GetPersistencyValues(DigitalEquipementPersistencyValues &persistencyValues) const; // override;
        void GetBytesFromData(std::vector<uint8_t> &data) const override;
        void SetDataFromBytes(std::vector<uint8_t> &data) override;
    };
}

#endif