#pragma once

#include <vector>
#include <functional>
#include <ModbusClientRTU.h>
#include "ModbusTask.h"
#include "ModbusRegisters.h"

#include "mmLogger.h"

class ModbusScheduler
{
public:
    ModbusScheduler(ModbusClientRTU &client);

    void setMaxParallel(uint8_t n);
    void addTask(const MBTask &task);
    std::vector<MBTask> &getTasks();

    void loop();

private:
    ModbusClientRTU &mb;
    std::vector<MBTask> tasks;
    uint8_t maxParallel = 1;
    uint32_t tokenCounter = 1;
    size_t nextIndex = 0;

    bool addModbusRequest(MBTask &t);
    void handleTimeouts();
    void startPendingTasks();

    void onData(ModbusMessage response, uint32_t token);
    void onError(Modbus::Error error, uint32_t token);

    MBTask *findTaskByToken(uint32_t token);
};
