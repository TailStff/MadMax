#ifndef PUMPSWAP_H
#define PUMPSWAP_H

#include "IPrimitive.h"
#include "mmFeedbackError.h"
#include "structFaults.h"
#include "mmRunTime.h"
#include "mmAccum.h"
#include "mmDigitalEquipment.h"
#include "helpersVector.h"
#include <vector>
#include <array>
#include "mmMiniScheduler.h"

namespace MadMax
{
    struct __attribute__((packed)) mmPumpSwapPersistencyValue
    {
        uint64_t runTime;
        uint64_t startCount;
    };

    struct mmPumpSwapPersistencyValues
    {
        std::vector<mmPumpSwapPersistencyValue> data;
    };

    // Enums value that indicate actual state of the system
    enum class enumCapacityState : uint8_t
    {
        Optimal = 0U,
        Degraded = 1U,
        Insufficient = 2U,
        Critical = 3U
    };

    // Minimal data for setting pump inputs
    struct SetPumpValue
    {
        bool Feedback;
        bool Fault;
    };

    struct PumpSwapResult
    {
        uint8_t AvailablePumps;
        uint8_t RequestedPumps;
        uint8_t RunningPumps;
        enumCapacityState CapacityState;
        uint8_t TotalPumps;

        // Structure that allow to get Pump Status
        std::vector<DigitalEquipmentStatus *> PumpsStatus;
    };

    class mmPumpSwap : public IPrimitiveTyped<mmPumpSwapPersistencyValues>, public IPrimitive
    {
    private:
        ExecutionEnv *executionEnv;

        // Number of equipement managed
        uint8_t count;

        // Delay before mark equipment in fault
        uint32_t feedbackDelay;

        std::vector<std::unique_ptr<mmDigitalEquipment>> nodes;
        std::vector<std::unique_ptr<SetPumpValue>> inputsValues;
        std::vector<std::unique_ptr<DigitalEquipmentStatus>> statuses;

        std::unique_ptr<mmMiniScheduler> miniScheduler;

        std::vector<int> indexes;
        std::vector<bool> physicalValues;
        std::vector<bool> memFaults;
        bool memReevaluation;

        // Variables for exposing unique_ptr outside
        // Pre-allocated to avoid heap fragmentation on each Evaluate() call
        std::vector<SetPumpValue *> rawInputs;
        std::vector<DigitalEquipmentStatus *> rawStatuses;

        // temporary vector used to keep tracing of already selected indexes
        std::vector<bool> selectedIndex;

        void computeIndexes(bool recomputeAllFlag = true);
        enumCapacityState GetCapacityState(uint8_t availablePumps, uint8_t requestedPumps);
        void GetBytesInto(std::vector<uint8_t> &data) const;

    public:
        // Constructors
        mmPumpSwap(ExecutionEnv *_executionEnv, uint8_t count, uint32_t feedbackDelay, mmPumpSwapPersistencyValues &persistancyValues);
        ~mmPumpSwap();

        void ComputePhysicalValues(std::initializer_list<bool> logicalValues);

        uint8_t GetCount() const;
        std::vector<bool> GetPhysicalValues() const;
        const DigitalEquipmentStatus &GetStatus(uint8_t index) const;

        // Evaluate function, here we user lambda with enclosure to give access to pumps inputs values while keeping access to env.
        PumpSwapResult Evaluate(std::function<void(std::vector<SetPumpValue *> &)> callbackFunction, std::initializer_list<bool> logicalValues, bool reevaluation, bool acknowledge);

        /// @brief Static function that give the size of the serialized data of the object, here we just serialize all pumps runtimes and start counts in a byte vector, so size is just count of pumps multiplied by size of runtime and start count values
        /// @param count Number of pumps managed by the object, used to compute size of serialized data
        /// @return Size in bytes of the serialized data of the object
        static size_t GetSerializedSize(uint8_t count)
        {
            return sizeof(mmPumpSwapPersistencyValue) * count;
        }

        void ResetAllRuntimes();
        void ResetAllStartCounts();

        void GetPersistencyValues(mmPumpSwapPersistencyValues &persistencyValues) const; // override;
        void GetBytesFromData(std::vector<uint8_t> &data) const override;
        void SetDataFromBytes(std::vector<uint8_t> &data) override;
    };
}

#endif