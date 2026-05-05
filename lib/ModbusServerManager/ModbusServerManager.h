#ifndef MADMAXMODBUSSERVERMANAGER_H
#define MADMAXMODBUSSERVERMANAGER_H

#include <cstdio>
#include <functional>
#include <vector>

#include "ModbusServerTCPasync.h"
#include "ModbusServerMemoryManager.h"
#include "ModbusServerWriteEvent.h"

namespace MadMax
{
    class ModbusServerManager
    {

    public:
        ModbusServerManager(ModbusServerMemoryManager &modbusServerMemoryManager);
        ~ModbusServerManager();

        using WriteCallback = std::function<void(const ModbusWriteEvent &)>;

        void RegisterWriteCallback(WriteCallback cb);

        ModbusMessage FC01(ModbusMessage &request);
        ModbusMessage FC03(ModbusMessage &request);
        ModbusMessage FC04(ModbusMessage &request);
        ModbusMessage FC06(ModbusMessage &request);
        ModbusMessage FC16(ModbusMessage &request);

        void RegisterWorkers();

        void Start(uint16_t port, uint16_t maxClients, uint16_t timeout);

    private:
        ModbusServerMemoryManager &modbusServerMemoryManager;

        // Create Modbus Server
        ModbusServerTCPasync MBserver;
        void RegisterWorker(uint8_t serverID, uint8_t functionCode, MBSworker worker);

        std::vector<WriteCallback> writeCallbacks;

        void NotifyWrite(uint16_t addr, uint16_t count)
        {
            ModbusWriteEvent evt{addr, count};
            for (auto &cb : writeCallbacks)
            {
                cb(evt);
            }
        }
    };
}

#endif