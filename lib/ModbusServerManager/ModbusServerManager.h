#include <cstdio>
#include "ModbusServerTCPasync.h"
#include "ModbusServerMemoryManager.h"

namespace MadMax
{
    class ModbusServerManager
    {
    private:
        ModbusServerMemoryManager &modbusServerMemoryManager;

        // Create Modbus Server
        ModbusServerTCPasync MBserver;
        void RegisterWorker(uint8_t serverID, uint8_t functionCode, MBSworker worker);

    public:
        ModbusServerManager(ModbusServerMemoryManager &modbusServerMemoryManager);
        ~ModbusServerManager();

        ModbusMessage FC01(ModbusMessage request);
        ModbusMessage FC03(ModbusMessage request);
        ModbusMessage FC04(ModbusMessage request);
        ModbusMessage FC06(ModbusMessage request);
        ModbusMessage FC16(ModbusMessage request);

        void RegisterWorkers();

        void Start(uint16_t port, uint16_t maxClients, uint16_t timeout);
    };
}