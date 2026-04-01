#pragma once
#include "ObjectProvider.h"
#include "IPersistency.h"
#include "mmVariable.h"
#include "mmDataType.h"
#include "mmVariableDTOMapper.h"

namespace MadMax
{
    class mmVariableProvider : public ObjectProvider<ISerializableBase>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

        void writePersistencyData(const std::string &name, ISerializableBase *obj)
        {
            // Get the bytes vector that represent the object persistency values
            std::vector<uint8_t> dataToWrite;
            obj->GetBytesFromData(dataToWrite);

            executionEnv->GetMiniPrefs()->Put(name.c_str(), dataToWrite.data(), dataToWrite.size());
        }

    public:
        mmVariableProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;

            dtoMappers = std::make_unique<mmVariableDTOMapper>();
        }

        template <class T>
        mmVariable<T> *Create(const std::string &name, int32_t address, mmVariablePersistencyValues<T> data = {.value = static_cast<T>(0)})
        {
            // We get the last saved value from memory
            GetPersistencyValuesFromMem(name, reinterpret_cast<uint8_t *>(&data), sizeof(mmVariablePersistencyValues<T>));

            // Copy the last saved value to modbus memory space, if it's valid, we consider that an address of -1 is an invalid address that mean that the variable is not associated to any Modbus register, this allow to create variable that are not exposed through Modbus if we want to
            if (address != -1)
            {
                // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
                uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;

                // Holding registers
                if (modbusMemorySpace == 4)
                {
                    // We associate the variable to the Modbus register using the modbus server manager, we also store the Modbus address in the provider to be able to retrieve it later if needed
                    uint16_t modbusAddress = address & 0x0000FFFF;

                    T *value = executionEnv->GetModbusServerManager()->AssociateHoldingRegister<T>(modbusAddress);
                    *value = data.value;
                }
            }

            // mmVariable<T> varies per T, so we cannot use ObjectProvider<mmVariable<T>> as base.
            // We must inject via the fixed base interface ObjectProvider<ISerializableBase>
            // to store all typed instances in a single polymorphic collection.
            auto *obj = new mmVariable<T>(executionEnv, data);
            ObjectProvider<ISerializableBase>::inject(name, address, obj);
            return obj;
        }

        /// @brief Getter function to get the desired object given by his name
        /// @tparam T Type of the variable to be retrieived
        /// @param name Name of the object to be retrieived
        /// @return Pointer to the retrieved object or nullptr if not found
        template <class T>
        mmVariable<T> *Get(const std::string &name) const
        {
            int32_t address;
            return static_cast<mmVariable<T> *>(ObjectProvider<ISerializableBase>::Get(name, address));
        }

        /// @brief Save the persistency values of the mmVariableFloat object with the given name
        /// @param name Name of the object to be serialized
        void SavePersistencyValuesToMem(const std::string &name) override
        {
            int32_t address;

            // Get the object to be serialized
            auto *obj = ObjectProvider<ISerializableBase>::Get(name, address);

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return;

            writePersistencyData(name, obj);
        }

        void GetPersistencyValuesFromMem(const std::string &name, uint8_t *data, size_t length)
        {
            uint16_t readedLength;
            uint16_t addr;
            executionEnv->GetMiniPrefs()->Get(name.c_str(), data, length, addr, readedLength);
        }

        /// @brief Function that SET new value to the variable and save persistency values if the value is different from the previous one
        /// @param name Name of the object to be updated
        /// @param value The new value to SET
        template <class T>
        void SetValue(const std::string &name, T value)
        {
            int32_t address;

            // Get the object to be serialized
            mmVariable<T> *obj = static_cast<mmVariable<T> *>(ObjectProvider<ISerializableBase>::Get(name, address));

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return;

            if (obj->SetValue(value))
                writePersistencyData(name, obj);
        }
        /*
            // API Parts

            void RefreshFromModbusRegisters()
            {
                this->ForEach([&](const std::string &name, IVariableValue *base)
                              {
                // We consider that only variables that are associated to Modbus registers need to be refreshed from Modbus registers, if the variable is not associated to any Modbus register, we consider that its value is managed internally and not updated from Modbus registers, so we skip it
                int32_t address;
                auto *obj = ObjectProvider<IVariableValue>::Get(name, address);

                if (address == -1)
                    return;

                // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
                uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;
                uint16_t modbusAddress = address & 0x0000FFFF;

                // Holding registers
                if (modbusMemorySpace == 4)
                {
                    // On lit selon le type réel de la variable
                    mmVariableValue incoming = std::visit([&](auto &&stored) -> mmVariableValue {
                        using TStored = std::decay_t<decltype(stored)>;

                        TStored *reg = executionEnv->GetModbusServerManager()
                                        ->AssociateHoldingRegister<TStored>(modbusAddress);
                        if (!reg) return stored; // pas de changement si pas de registre

                        return mmVariableValue{*reg};
                    }, base->GetVariantValue());

                    if (base->SetVariantValue(incoming))
                        writePersistencyData(name, base);
                } });
            }*/

#pragma region IProviderDTO
        std::vector<DTOBase> GetDTOs() const override
        {
            std::vector<DTOBase> result;
            result.reserve(this->size());

            this->ForEach(
                [&](const std::string &name, ISerializableBase *base)
                {
                    DTOBase dto;
                    if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            ISerializableBase *obj = ObjectProvider<ISerializableBase>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
            {
                dto.objectName = name;
            }

            // IPrimitive *primitive = static_cast<IPrimitive *>(obj);
            // primitive->GetDTO(dto);
            // dto.objectName = name;

            return true;
        }

        /// @brief Function that fill the given DTO with the detailed data of the object, this will be used to expose object data through API in a generic way without needing to know the object type, we will just use the DTOBase fields to expose data, this function can be used to expose more detailed data than GetDTO function
        /// @param name Name of the object to get details
        /// @param dto Reference to the DTO that will receive the object data
        /// @return Return true if the DTO was filled successfully, false if there is an error during data retrieval
        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            ISerializableBase *obj = ObjectProvider<ISerializableBase>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
            {
                dto.objectName = name;
            }

            /*
        IPrimitive *primitive = static_cast<IPrimitive *>(obj);
        primitive->GetDetailDTO(dto);
        dto.objectName = name;*/

            return true;
        }

#pragma endregion IProviderDTO
    };
}