#include "ModbusScheduler.h"

MBTask::MBTask()
{
}

MBTask::~MBTask()
{
}

void MBTask::Set(uint8_t slave, MBFunction function, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, uint16_t *writeValuePtr, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = function;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = writeValuePtr;
    this->callback = callback;
}

void MBTask::Set2(uint8_t slave, MBFunction function, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, uint8_t *writeValuePtr, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = function;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->coilsValuePtr = writeValuePtr;
    this->callback = callback;
}

ModbusScheduler::ModbusScheduler(ModbusClientRTU &client) : mb(client)
{
    mb.onDataHandler([this](ModbusMessage response, uint32_t token)
                     { onData(response, token); });

    mb.onErrorHandler([this](Modbus::Error error, uint32_t token)
                      { onError(error, token); });
}

void ModbusScheduler::setMaxParallel(uint8_t n)
{
    maxParallel = n;
}

void ModbusScheduler::addTask(const MBTask &task)
{
    tasks.push_back(task);
}

std::vector<MBTask> &ModbusScheduler::getTasks()
{
    return tasks;
}

void ModbusScheduler::loop()
{
    handleTimeouts();
    startPendingTasks();
}

void ModbusScheduler::handleTimeouts()
{
    try
    {
        uint32_t now = millis();

        for (std::vector<MBTask>::iterator it = tasks.begin(); it != tasks.end(); ++it)
        {
            MBTask &t = *it;

            if (!t.inProgress)
                continue;

            if (now - t.requestStartTime < t.timeoutMs)
                continue;

            t.inProgress = false;
            t.state = MBTaskState::Timeout;

            if (t.retryCount < t.retryMax)
            {
                t.retryCount++;
                t.nextPollTime = now + 1000;
            }
            else
            {
                t.retryCount = 0;
                t.nextPollTime = now + t.pollIntervalMs;
            }

            if (t.callback)
                t.callback(t);
        }
    }
    catch (const std::exception &e)
    {
        Serial.println("Error response: " + String(e.what()));
    }
}

void ModbusScheduler::startPendingTasks()
{
    if (tasks.empty())
        return;

    uint32_t now = millis();

    // Compter les tâches déjà actives
    uint8_t active = 0;
    for (const MBTask &t : tasks)
        if (t.inProgress)
            active++;

    if (active >= maxParallel)
        return;

    size_t count = tasks.size();
    size_t scanned = 0;

    // Parcours circulaire à partir de nextIndex
    while (scanned < count && active < maxParallel)
    {
        MBTask &t = tasks[nextIndex];

        if (!t.inProgress && (now >= t.nextPollTime || t.TriggerRequest))
        {
            sendRequest(t);

            t.inProgress = true;
            t.requestStartTime = now;
            t.state = MBTaskState::Running;

            active++;
        }

        // avance circulaire
        nextIndex = (nextIndex + 1) % count;
        scanned++;
    }
}

void ModbusScheduler::sendRequest(MBTask &t)
{
    uint32_t token = tokenCounter++;

    Modbus::Error err = Modbus::SUCCESS;

    t.token = token;

    switch (t.function)
    {
    case MBFunction::ReadHoldingRegisters:
    case MBFunction::ReadInputRegisters:
    case MBFunction::ReadCoils:
    case MBFunction::ReadDiscreteInputs:
        err = mb.addRequest(token, t.slave, (uint8_t)t.function, t.address, t.quantity);
        break;

    case MBFunction::WriteSingleCoil:
    case MBFunction::WriteSingleRegister:
    {
        uint16_t value = t.writeValuePtr ? *t.writeValuePtr : 0;
        err = mb.addRequest(token, t.slave, (uint8_t)t.function, t.address, value);
    }
    break;

    case MBFunction::WriteMultipleCoils:
    {
        // Tested ok on AS-P
        uint8_t byteQuantity = (t.quantity + 7) / 8; // get number of bytes needed for coils in request, ceil(t.quantity / 8.0) integer
        err = mb.addRequest(token, t.slave, (uint8_t)t.function, t.address, t.quantity, byteQuantity, t.coilsValuePtr);
    }
    break;
    case MBFunction::WriteMultipleRegisters:
        // Tested ok on AS-P
        err = mb.addRequest(token, t.slave, (uint8_t)t.function, t.address, t.quantity, t.quantity * 2, t.writeValuePtr);
        break;
    }

    if (err != Modbus::SUCCESS)
    {
        t.inProgress = false;
        t.state = MBTaskState::Error;
        t.nextPollTime = millis() + t.pollIntervalMs;

        if (t.callback)
            t.callback(t);
    }
}

MBTask *ModbusScheduler::findTaskByToken(uint32_t token)
{
    // mapping simple : token order == running order
    // améliorable avec map token->task*
    for (auto &t : tasks)
        if (t.token == token)
            return &t;

    return nullptr;
}

void ModbusScheduler::onData(ModbusMessage response, uint32_t token)
{
    MBTask *t = findTaskByToken(token);
    if (!t)
        return;

    t->inProgress = false;
    t->retryCount = 0;
    t->state = MBTaskState::Done;
    t->nextPollTime = millis() + t->pollIntervalMs;

    t->readData.clear();

    uint8_t fc = response.getFunctionCode();

    if (fc == 3 || fc == 4)
    {
        uint8_t count = response[2] / 2;
        for (uint8_t i = 0; i < count; i++)
        {
            uint16_t v = (response[3 + i * 2] << 8) | response[4 + i * 2];
            t->readData.push_back(v);
        }
    }

    if (t->callback)
        t->callback(*t);

    if (t->TriggerRequest)
        t->TriggerRequest = false;
}

void ModbusScheduler::onError(Modbus::Error error, uint32_t token)
{
    MBTask *t = findTaskByToken(token);
    if (!t)
        return;

    t->inProgress = false;
    t->state = MBTaskState::Error;

    if (t->retryCount < t->retryMax)
    {
        t->retryCount++;
        t->nextPollTime = millis() + 100;
    }
    else
    {
        t->retryCount = 0;
        t->nextPollTime = millis() + t->pollIntervalMs;
    }

    if (t->callback)
        t->callback(*t);
}
