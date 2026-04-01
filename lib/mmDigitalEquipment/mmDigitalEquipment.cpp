#include "mmDigitalEquipment.h"

namespace MadMax
{
    mmDigitalEquipment::mmDigitalEquipment(ExecutionEnv *_executionEnv, mmDigitalEquipementPersistencyValues data) : executionEnv(_executionEnv), memAcknowledge(false)
    {
        feedbackErrorObj = new mmFeedbackError(executionEnv);
        runTimeObj = new mmRunTime(executionEnv, {.value = data.runTime});
        accumObj = new mmAccum<uint64_t>(executionEnv, {.value = data.startCount});

        this->status.feedbackFault = false;
        this->status.output = false;
    }

    mmDigitalEquipment::~mmDigitalEquipment()
    {
        delete feedbackErrorObj;
        delete runTimeObj;
        delete accumObj;
    }

    bool mmDigitalEquipment::Evaluate(bool command, bool feedback, bool fault, bool acknowledge, uint32_t feedbackDelay, DigitalEquipmentStatus *outStatus)
    {
        this->status.command = command;
        this->status.feedback = feedback;
        this->status.fault = fault;

        bool reset = acknowledge && !memAcknowledge;

        // SET fault has priority over acknowledge reset
        bool fbError = feedbackErrorObj->Evaluate(command, feedback, feedbackDelay, 0, reset, FeedbackErrorOption::OnlyOn);
        if (fbError && !this->status.feedbackFault)
            this->status.feedbackFault = true;
        else if (reset)
            this->status.feedbackFault = false;

        this->status.runTimeValue = runTimeObj->Evaluate(feedback);
        this->status.startCountValue = accumObj->Evaluate(feedback, 1);

        bool tmpFault = this->status.feedbackFault || fault;

        this->status.output = command && !tmpFault;

        if (outStatus)
        {
            outStatus->command = command;
            outStatus->feedback = feedback;
            outStatus->fault = fault;
            outStatus->feedbackFault = this->status.feedbackFault;
            outStatus->output = this->status.output;
            // outStatus->faults.fault = tmpFault;
            // outStatus->faults.ackFault = feedbackFault;
            // outStatus->faults.criticalFault = tmpFault;
            outStatus->runTimeValue = this->status.runTimeValue;
            outStatus->startCountValue = this->status.startCountValue;
        }

        memAcknowledge = acknowledge;

        return this->status.output;
    }

    bool mmDigitalEquipment::GetValue() const
    {
        return this->status.output;
    }

    const DigitalEquipmentStatus &mmDigitalEquipment::GetStatus() const
    {
        return this->status;
    }

    void mmDigitalEquipment::GetPersistencyValues(mmDigitalEquipementPersistencyValues &persistencyValues) const
    {
        persistencyValues.runTime = this->status.runTimeValue;
        persistencyValues.startCount = this->status.startCountValue;
    }

    void mmDigitalEquipment::SetRuntime(uint64_t runtime)
    {
        this->runTimeObj->Reset(runtime);
    }

    void mmDigitalEquipment::SetStartCount(uint64_t startCount)
    {
        this->accumObj->Reset(startCount);
    }

#pragma region IPersistable
    /// @brief Get the bytes vector that represent the object persistency values, here we just serialize all pumps runtimes and start counts in a byte vector
    /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
    void mmDigitalEquipment::GetBytesFromData(std::vector<uint8_t> &data) const
    {
        mmDigitalEquipementPersistencyValues persistencyValues;
        GetPersistencyValues(persistencyValues);

        size_t size = sizeof(persistencyValues);
        data.resize(size);
        memcpy(data.data(), &persistencyValues, size);

        return;
    }

    void mmDigitalEquipment::SetDataFromBytes(std::vector<uint8_t> &data)
    {
        if (data.size() != sizeof(mmDigitalEquipementPersistencyValues))
            return;

        mmDigitalEquipementPersistencyValues persistencyValues;
        memcpy(&persistencyValues, data.data(), sizeof(mmDigitalEquipementPersistencyValues));

        this->status.runTimeValue = persistencyValues.runTime;
        this->status.startCountValue = persistencyValues.startCount;
    }
#pragma endregion IPersistable
}