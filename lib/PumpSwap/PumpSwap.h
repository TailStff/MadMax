#ifndef MADMAXPUMPSWAP_H
#define MADMAXPUMPSWAP_H

#include <vector>
#include <array>

#include "ISerializable.h"
#include "DigitalEquipment.h"
#include "helpersVector.h"
#include "MiniScheduler.h"

namespace MadMax
{
    struct __attribute__((packed)) PumpSwapPersistencyValue
    {
        uint64_t runTime;
        uint64_t startCount;
    };

    struct PumpSwapPersistencyValues
    {
        std::vector<PumpSwapPersistencyValue> data;
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

    class PumpSwap : public IPrimitive, public ISerializable
    {
    private:
        ExecutionEnv *executionEnv;

        // Number of equipement managed, setted at construction
        const uint8_t count;

        // Delay before mark equipment in fault
        uint32_t feedbackDelay;

        bool externalAcknowledgeBit = false;

        std::vector<std::unique_ptr<DigitalEquipment>> nodes;
        std::vector<std::unique_ptr<SetPumpValue>> inputsValues;
        std::vector<std::unique_ptr<DigitalEquipmentStatus>> statuses;

        std::unique_ptr<MiniScheduler> miniScheduler;

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

        std::mutex mutex;

        void computeIndexes(bool recomputeAllFlag = true);
        enumCapacityState getCapacityState(uint8_t availablePumps, uint8_t requestedPumps);
        void GetBytesInto(std::vector<uint8_t> &data) const;

        PumpSwapResult pumpSwapResult;

        void resetAllStartCounts();
        void resetAllRuntimes();

        void retreivePhysicalValues(std::initializer_list<bool> logicalValues);

    public:
        // Constructors
        PumpSwap(ExecutionEnv *_executionEnv, uint8_t count, uint32_t feedbackDelay, PumpSwapPersistencyValues &persistancyValues);
        ~PumpSwap();

        std::vector<bool> GetPhysicalValues() const;
        const DigitalEquipmentStatus &GetStatus(uint8_t index) const;
        const PumpSwapResult GetPumpSwapResult() const;

        /// @brief Function used to EVALUATE the pump swap object,
        /// @param callbackFunction Callback function used to pass dynamically pumps feedback and fault
        /// @param logicalValues List of logical values, meaning false or true for run pumps
        /// @param reevaluation Boolean flag to RE-EVALUATE all priorities without keeping any previous working states
        /// @param acknowledge Boolean flag to ACKNOWLEDGE any faults
        /// @return PumpSwapResult object that describe all properties
        PumpSwapResult Evaluate(std::function<void(std::vector<SetPumpValue *> &)> callbackFunction, std::initializer_list<bool> logicalValues, bool reevaluation, bool acknowledge);

        /// @brief Static function that give the size of the serialized data of the object, here we just serialize all pumps runtimes and start counts in a byte vector, so size is just count of pumps multiplied by size of runtime and start count values
        /// @param count Number of pumps managed by the object, used to compute size of serialized data
        /// @return Size in bytes of the serialized data of the object
        static size_t GetSerializedSize(uint8_t count)
        {
            return sizeof(PumpSwapPersistencyValue) * count;
        }

        /// @brief Function that allow to reset all runtimes and start counts of the pumps, this function will be called by the user when he want to reset all runtimes and start counts
        /// @note This function is thread safe, it will lock the mutex before resetting all runtimes and start counts
        void ResetAllRuntimes();

        /// @brief Function that allow to reset all start counts of the pumps, this function will be called by the user when he want to reset all start counts
        /// @note This function is thread safe, it will lock the mutex before resetting all start counts
        void ResetAllStartCounts();

        /// @brief Function that allow to reset all runtimes and start counts of the pumps, this function will be called by the user when he want to reset all runtimes and start counts
        /// @note This function is thread safe, it will lock the mutex before resetting all runtimes and start counts
        void TriggerResetAction();

        /// @brief Function that allow to acknowledge all feedback faults of the pumps, this function will be called by the user when he want to acknowledge all faults and feedback faults
        /// @note This function is thread safe, it will lock the mutex before acknowledging all feedback faults
        void TriggerAcknowledge();

        void GetPersistencyValues(PumpSwapPersistencyValues &persistencyValues) const; // override;
        void GetBytesFromData(std::vector<uint8_t> &data) const override;
        void SetDataFromBytes(std::vector<uint8_t> &data) override;

        const ISerializable *AsSerializable() const override { return this; }
        ISerializable *AsSerializable() override { return this; }

        const PumpSwap *AsPumpSwap() const { return this; }
        PumpSwap *AsPumpSwap() { return this; }
    };
}

#endif