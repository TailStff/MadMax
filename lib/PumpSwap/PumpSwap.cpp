#include "PumpSwap.h"

namespace MadMax
{
    PumpSwap::PumpSwap(ExecutionEnv *_executionEnv, uint8_t count, uint32_t feedbackDelay, PumpSwapPersistencyValues &persistancyValues)
    {
        this->executionEnv = _executionEnv;
        this->count = count;
        this->feedbackDelay = feedbackDelay;

        memFaults.assign(count, false);
        physicalValues.assign(count, false);

        nodes.reserve(count);
        inputsValues.reserve(count);
        statuses.reserve(count);

        // we initialize our objects necessary to expose object to outside
        // Pre-allocated to avoid heap fragmentation on each Evaluate() call
        rawInputs.reserve(count);
        rawStatuses.reserve(count);

        selectedIndex.resize(count);

        miniScheduler = std::make_unique<MiniScheduler>(_executionEnv);

        // Default orders
        indexes.assign(count, -1);

        for (uint8_t i = 0; i < count; i++)
        {
            DigitalEquipementPersistencyValues tempPersistencyValue{.runTime = persistancyValues.data[i].runTime, .startCount = persistancyValues.data[i].startCount};

            // Create via make_unique as mmPumpSwap is taking ownership of thoses object (no need to share thoses objects via pointer)
            nodes.push_back(std::make_unique<DigitalEquipment>(_executionEnv, tempPersistencyValue));

            // Create unique object via make unique
            inputsValues.push_back(std::make_unique<SetPumpValue>());

            statuses.push_back(std::make_unique<DigitalEquipmentStatus>(DigitalEquipmentStatus{.runTimeValue = persistancyValues.data[i].runTime, .startCountValue = persistancyValues.data[i].startCount}));
        }

        computeIndexes();
    }

    PumpSwap::~PumpSwap()
    {
        // memFaults is deleted by itself
        // physicalValues is deleted by itself
        // indexes is deleted by itself
        // miniScheduler is deleted by itself
        // inputsValues is deleted by itself
        // statuses is deleted by itself
    }

    uint8_t PumpSwap::GetCount() const
    {
        return this->count;
    }

    std::vector<bool> PumpSwap::GetPhysicalValues() const
    {
        return this->physicalValues;
    }

    const DigitalEquipmentStatus &PumpSwap::GetStatus(uint8_t index) const
    {
        return *this->statuses.at(index);
    }

    const PumpSwapResult PumpSwap::GetPumpSwapResult() const
    {
        return pumpSwapResult;
    }

    /// @brief Evaluate function, here we user lambda with enclosure to give access to pumps inputs values while keeping access to env.
    /// @param callbackFunction Callback lambda function that allow user to inject values
    /// @param logicalValues List of values to apply
    /// @param reevaluation Flag that force to recompute indexes
    /// @param acknowledge Flag that
    /// @return mmPumpSwap value that give all pumps informations
    PumpSwapResult PumpSwap::Evaluate(std::function<void(std::vector<SetPumpValue *> &)> callbackFunction, std::initializer_list<bool> logicalValues, bool reevaluation, bool acknowledge)
    {
        // We compute the number of pumps that are needed for optimal operation
        uint8_t requestedPumps = HelpersVectors::countTrue(logicalValues);

        // Compute physical values
        ComputePhysicalValues(logicalValues);

        // rawInputs.clear();
        // for (auto &ptr : inputsValues)
        //     rawInputs.push_back(ptr.get());

        // Populate pre-allocated rawInputs to avoid heap fragmentation
        for (size_t i = 0; i < count; i++)
            rawInputs[i] = inputsValues[i].get();

        // Call lambda callback with inputs values
        callbackFunction(rawInputs);

        uint8_t availablePumps = 0;
        uint8_t runningPumps = 0;

        // Flag that indicate that an actually running pump failed
        bool flagFault = false;

        // Flag that indicate that one or more pumps was recovered from fail state
        bool flagRecovery = false;

        bool flagReevaluation = reevaluation && !memReevaluation;

        for (uint8_t i = 0; i < count; i++)
        {
            nodes[i]->Evaluate(physicalValues[i], inputsValues[i]->Feedback, inputsValues[i]->Fault, acknowledge, feedbackDelay, statuses[i].get());

            bool synDef = statuses[i]->fault || statuses[i]->feedbackFault;

            if (!flagFault && physicalValues[i] && synDef && !memFaults[i])
                flagFault = true;

            if (!flagRecovery && !synDef && memFaults[i])
                flagRecovery = true;

            if (!synDef)
                availablePumps++;

            if (physicalValues[i])
                runningPumps++;

            memFaults[i] = synDef;
        }

        auto dateTime = DateTimeDefinition{.Year = ANY, .Month = ANY, .Day = ANY, .DayOfWeek = ANY, .Hour = ANY, .Minute = ANY, .Second = 0};

        bool permut = miniScheduler->Evaluate(dateTime);

        // We re-compute indexes if running pump is in fault, if a pump recover from fail state or if we have an external request
        if (flagFault || flagReevaluation || (flagRecovery && runningPumps < requestedPumps) || permut)
            computeIndexes(permut);

        enumCapacityState capacityState = GetCapacityState(availablePumps, requestedPumps);

        memReevaluation = reevaluation;

        rawStatuses.clear();
        for (auto &ptr : statuses)
            rawStatuses.push_back(ptr.get());

        // Set pumpSwapResult return value
        pumpSwapResult.AvailablePumps = availablePumps;
        pumpSwapResult.RequestedPumps = requestedPumps;
        pumpSwapResult.RunningPumps = runningPumps;
        pumpSwapResult.CapacityState = capacityState;
        pumpSwapResult.TotalPumps = count;
        pumpSwapResult.PumpsStatus = rawStatuses;

        return pumpSwapResult;
    }

    /// @brief Function that allow to RECOMPUTE pumps orders by their actuals states and their runtimes
    /// @param recomputeAllFlag Flag that authorize to recompute all working pumps without taking thoses that were running before
    void PumpSwap::computeIndexes(bool recomputeAllFlag)
    {
        // Array that keep selection state, all already selected won't be selected more than once
        std::fill(selectedIndex.begin(), selectedIndex.end(), false);

        uint8_t startSearch = 0;

        if (recomputeAllFlag)
        {
            // Set all indexes to -1 (no value)
            for (uint8_t i = 0; i < count; i++)
                indexes[i] = -1;
        }
        else
        {
            for (uint8_t i = 0; i < count; i++)
            {
                bool synDef = this->statuses[indexes[i]]->fault || this->statuses[indexes[i]]->feedbackFault;

                if (indexes[i] != -1 && statuses[indexes[i]]->output && !synDef && !selectedIndex[indexes[i]])
                {
                    indexes[startSearch++] = indexes[i];
                    selectedIndex[indexes[i]] = true;
                }
            }
        }

        // Iteration for candidates
        for (uint8_t i = startSearch; i < count; i++)
        {
            int nextCandidate = -1;
            uint64_t minimalRT = std::numeric_limits<uint64_t>::max();
            for (uint8_t j = 0; j < count; j++)
            {
                bool synDef = statuses[j]->fault || statuses[j]->feedbackFault;

                if (!synDef && !selectedIndex[j] && this->statuses[j]->runTimeValue < minimalRT)
                {
                    nextCandidate = j;
                    minimalRT = this->statuses[j]->runTimeValue;
                }
            }

            if (nextCandidate != -1)
            {
                indexes[i] = nextCandidate;
                selectedIndex[nextCandidate] = true;
            }
            else
                indexes[i] = -1;
        }
    }

    void PumpSwap::ComputePhysicalValues(std::initializer_list<bool> logicalValues)
    {
        // Reset all physical values
        for (uint8_t i = 0; i < count; i++)
            physicalValues[i] = false;

        int i = 0;
        for (const auto value : logicalValues)
        {
            // If -1 value is find, no need to go further
            if (indexes[i] == -1)
                break;

            // Set the value
            physicalValues[indexes[i++]] = value;
        }
    }

    enumCapacityState PumpSwap::GetCapacityState(uint8_t availablePumps, uint8_t requestedPumps)
    {
        if (availablePumps == count)
            return enumCapacityState::Optimal;

        if (availablePumps >= requestedPumps)
            return enumCapacityState::Degraded;

        if (availablePumps == 0)
            return enumCapacityState::Critical;

        return enumCapacityState::Insufficient;
    }

    /// @brief Get the persistency values of the object, here we just get all pumps runtimes and start counts and put thoses values in a vector
    /// @param persistencyValues Object that will receive persistency values
    /// @return true if we have an error during persistency values retrieval, false otherwise
    void PumpSwap::GetPersistencyValues(PumpSwapPersistencyValues &persistencyValues) const
    {
        persistencyValues.data.clear();
        for (uint8_t i = 0; i < count; i++)
            persistencyValues.data.push_back({.runTime = statuses[i]->runTimeValue, .startCount = statuses[i]->startCountValue});
    }

    void PumpSwap::ResetAllStartCounts()
    {
        for (uint8_t i = 0; i < count; i++)
            nodes[i]->SetStartCount(0);
    }

    void PumpSwap::ResetAllRuntimes()
    {
        for (uint8_t i = 0; i < count; i++)
            nodes[i]->SetRuntime(0);
    }

    /// @brief Serialize persistency values into a byte vector
    /// @param data Output vector that will receive the serialized bytes
    void PumpSwap::GetBytesFromData(std::vector<uint8_t> &data) const
    {
        PumpSwapPersistencyValues persistencyValues;
        GetPersistencyValues(persistencyValues);

        size_t size = persistencyValues.data.size() * sizeof(PumpSwapPersistencyValue);
        data.resize(size);

        if (!persistencyValues.data.empty())
            memcpy(data.data(), persistencyValues.data.data(), size);
    }

    void PumpSwap::SetDataFromBytes(std::vector<uint8_t> &data)
    {
        if (data.size() != GetSerializedSize(count))
            return;

        PumpSwapPersistencyValues persistencyValues;
        memcpy(&persistencyValues, data.data(), sizeof(PumpSwapPersistencyValues));

        for (uint8_t i = 0; i < count; i++)
        {
            // nodes[i]->SetPersistencyValues(&persistencyValues.data[0]);
        }
    }
}