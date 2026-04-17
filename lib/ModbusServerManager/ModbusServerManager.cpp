#include "ModbusServerManager.h"

namespace MadMax
{
    ModbusServerManager::ModbusServerManager(ModbusServerMemoryManager &modbusServerMemoryManager) : modbusServerMemoryManager(modbusServerMemoryManager)
    {
    }

    ModbusServerManager::~ModbusServerManager()
    {
    }

    void ModbusServerManager::RegisterWorker(uint8_t serverID, uint8_t functionCode, MBSworker worker)
    {
        MBserver.registerWorker(serverID, functionCode, worker);
    }

    void ModbusServerManager::Start(uint16_t port, uint16_t maxClients, uint16_t timeout)
    {
        MBserver.start(port, maxClients, timeout);
    }

    // Some functions to be called when function codes 0x01, 0x05 or 0x15 are requested
    // FC_01: act on 0x01 requests - READ_COIL
    ModbusMessage ModbusServerManager::FC01(ModbusMessage &request)
    {
        ModbusMessage response;
        // Request parameters are first coil and number of coils to read
        uint16_t start = 0;
        uint16_t numCoils = 0;
        request.get(2, start, numCoils);

        // Are the parameters valid?
        if (modbusServerMemoryManager.CheckCoilOverFlow(start, numCoils))
        {
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
            return response;
        }

        // Looks like it. Get the requested coils from our storage
        vector<uint8_t> coilset = modbusServerMemoryManager.GetCoilsPtr()->slice(start, numCoils);
        // Set up response according to the specs: serverID, function code, number of bytes to follow, packed coils
        response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)coilset.size(), coilset);

        // Return the response
        return response;
    }

    // Server function to handle FC 0x03 (FC03) - Read Holding Registers
    ModbusMessage ModbusServerManager::FC03(ModbusMessage &request)
    {

        ModbusMessage response; // The Modbus message we are going to give back
        uint16_t addr = 0;      // Start address
        uint16_t words = 0;     // # of words requested
        request.get(2, addr);   // read address from request
        request.get(4, words);  // read # of words from request

        // Address overflow?
        if (modbusServerMemoryManager.CheckHoldingRegisterOverFlow(addr, words))
        {
            // Yes - send respective error response
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
            return response;
        }

        // Set up response
        response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));
        modbusServerMemoryManager.addHoldingRegisters(&response, addr, words);

        // Send response back
        return response;
    }

    // Server function to handle FC 0x04 (FC04) - Read Input Registers
    ModbusMessage ModbusServerManager::FC04(ModbusMessage &request)
    {

        ModbusMessage response; // The Modbus message we are going to give back
        uint16_t addr = 0;      // Start address
        uint16_t words = 0;     // # of words requested
        request.get(2, addr);   // read address from request
        request.get(4, words);  // read # of words from request

        // Address overflow?
        if (modbusServerMemoryManager.CheckInputRegisterOverFlow(addr, words))
        {
            // Yes - send respective error response
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
            return response;
        }

        // Set up response
        response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));
        modbusServerMemoryManager.addInputRegisters(&response, addr, words);

        // Send response back
        return response;
    }

    // Server function to handle FC 0x06 (FC06) - Write Single Register
    ModbusMessage ModbusServerManager::FC06(ModbusMessage &request)
    {

        ModbusMessage response; // The Modbus message we are going to give back
        uint16_t addr = 0;      // Start address
        uint16_t val = 0;       // value to write
        request.get(2, addr);   // read address from request
        request.get(4, val);    // read value from request

        // Address overflow?
        if (modbusServerMemoryManager.CheckHoldingRegisterOverFlow(addr, 1))
        {
            // Yes - send respective error response
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
            return response;
        }

        // Set up response
        *(modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(addr)) = val;
        response.add(request.getServerID(), request.getFunctionCode(), addr, val);

        // Send response back
        return response;
    }

    // Server function to handle FC 0x10 (FC16) - Write Multiple Registers
    ModbusMessage ModbusServerManager::FC16(ModbusMessage &request)
    {
        ModbusMessage response; // The Modbus message we are going to give back
        uint16_t addr = 0;      // Start address
        uint16_t words = 0;     // total words to write
        uint8_t bytes = 0;      // # of data bytes in request
        uint16_t val = 0;       // value to be written
        request.get(2, addr);   // read address from request
        request.get(4, words);  // read # of words from request
        request.get(6, bytes);  // read # of data bytes from request (seems redundant with # of words)

        // # of registers proper?
        if ((bytes != (words * 2)) // byte count in request must match # of words in request
            || (words > 123))      // can't support more than this in request packet
        {                          // Yes - send respective error response
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_VALUE);
            return response;
        }
        // Address overflow?
        if (modbusServerMemoryManager.CheckHoldingRegisterOverFlow(addr, words))
        {
            // Yes - send respective error response
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
            return response;
        }

        // Do the writes
        for (uint16_t i = 0; i < words; ++i)
        {
            request.get(7 + (i * 2), val); // read value from request
            *(modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(addr + i)) = val;
        }

        // Set up response
        response.add(request.getServerID(), request.getFunctionCode(), addr, words);
        return response;
    }

    void ModbusServerManager::RegisterWorkers()
    {
        RegisterWorker(1, READ_COIL, [this](auto req)
                       { return FC01(req); }); // FC=01 for serverID = 1
        RegisterWorker(1, READ_HOLD_REGISTER, [this](auto req)
                       { return FC03(req); }); // FC=03 for serverID = 1
        RegisterWorker(1, READ_INPUT_REGISTER, [this](auto req)
                       { return FC04(req); }); // FC=04 for serverID = 1
        RegisterWorker(1, WRITE_HOLD_REGISTER, [this](auto req)
                       { return FC06(req); }); // FC=06 for serverID = 1
        RegisterWorker(1, WRITE_MULT_REGISTERS, [this](auto req)
                       { return FC16(req); }); // FC=16 for serverID = 1
    }
}