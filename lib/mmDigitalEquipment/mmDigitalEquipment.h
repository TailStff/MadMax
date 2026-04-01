#ifndef DIGITALEQUIPMENT_H
#define DIGITALEQUIPMENT_H

#include "mmDigitalEquipmentStatus.h"
#include "IPrimitive.h"
#include "mmFeedbackError.h"
#include "structFaults.h"
#include "mmRunTime.h"
#include "mmAccum.h"

struct mmDigitalEquipementPersistencyValues
{
    uint64_t runTime;
    uint64_t startCount;
};

class mmDigitalEquipment : public IPrimitiveTyped<mmDigitalEquipementPersistencyValues>, public IPrimitive
{
private:
    ExecutionEnv *executionEnv;
    mmFeedbackError *feedbackErrorObj;
    mmRunTime *runTimeObj;
    mmAccum<uint64_t> *accumObj;

    DigitalEquipmentStatus status;

    bool memAcknowledge;

public:
    // Constructors
    mmDigitalEquipment(ExecutionEnv *_executionEnv, mmDigitalEquipementPersistencyValues data = {.runTime = 0UL, .startCount = 0UL});
    ~mmDigitalEquipment();

    bool Evaluate(bool cmd, bool feedback, bool fault, bool acknowledge, uint32_t feedbackDelay, DigitalEquipmentStatus *outStatus = nullptr);

    const DigitalEquipmentStatus &GetStatus() const;
    bool GetValue() const;

    void SetRuntime(uint64_t runtime);
    void SetStartCount(uint64_t startCount);

    /// @brief Static function that give the size of the serialized data of the object, here we just serialize runTime and startCount Values
    /// @return Size in bytes of the serialized data of the object
    static size_t GetSerializedSize()
    {
        return sizeof(mmDigitalEquipementPersistencyValues);
    }

    void GetPersistencyValues(mmDigitalEquipementPersistencyValues &persistencyValues) const; // override;
    void GetBytesFromData(std::vector<uint8_t> &data) const override;
    void SetDataFromBytes(std::vector<uint8_t> &data) override;
};

#endif