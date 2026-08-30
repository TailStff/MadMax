#pragma once

#include "ObjectProvider.h"
#include "Variable.h"
#include "DataType.h"
#include "VariableDTOMapper.h"
#include "VariableModbusMapper.h"
#include "ModbusBinding.h"

#include "mmLogger.h"

#define MQTT_TOPIC_PREFIX "MadMax2/instance/Variables/"

namespace MadMax
{
    class VariableProvider : public ObjectProvider<IPrimitive>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMapper;

        /// @brief Function to write persistency values of the given ISerializable object to persistancy memory
        /// @param name Name of the object to be serialized
        /// @param obj Pointer to the object to be serialized
        void writePersistencyData(const std::string &name, const ISerializable *obj)
        {
            // Get the bytes vector that represent the object persistency values
            std::vector<uint8_t> dataToWrite;
            obj->GetBytesFromData(dataToWrite);

            executionEnv->GetMiniPrefs()->Put(name.c_str(), dataToWrite.data(), dataToWrite.size());
        }

        bool checkCollision(uint16_t variableModbusAddress, uint16_t variableLength, uint16_t addr, uint16_t count)
        {
            uint16_t varStart = variableModbusAddress;
            uint16_t varLengthWords = (variableLength + 1) / 2;
            uint16_t varEnd = varStart + varLengthWords;

            uint16_t writeStart = addr;
            uint16_t writeEnd = addr + count;

            return (writeStart < varEnd && writeEnd > varStart);
        }

    public:
        VariableProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMapper = std::make_unique<VariableDTOMapper>();
        }

        template <class T>
        Variable<T> *Create(const std::string &name, VariablePersistencyValues<T> data = {.value = static_cast<T>(0)})
        {
            // We get the last saved value from memory
            GetPersistencyValuesFromStorage(name, reinterpret_cast<uint8_t *>(&data), sizeof(VariablePersistencyValues<T>));

            // mmVariable<T> varies per T, so we cannot use ObjectProvider<mmVariable<T>> as base.
            // We must inject via the fixed base interface ObjectProvider<ISerializableBase>
            // to store all typed instances in a single polymorphic collection.
            auto *obj = new Variable<T>(executionEnv, data);
            ObjectProvider<IPrimitive>::inject(name, obj);

            // modbusMapper && modbusMapper->ExposeToModbus(*obj, address);

            return obj;
        }

        /// @brief Getter function to get the desired object given by his name
        /// @tparam T Type of the variable to be retrieived
        /// @param name Name of the object to be retrieived
        /// @return Pointer to the retrieved object or nullptr if not found
        template <class T>
        Variable<T> *Get(const std::string &name) const
        {
            return static_cast<Variable<T> *>(ObjectProvider<IPrimitive>::Get(name));
        }

        /// @brief Function that SET new value to the variable and save persistency values if the value is different from the previous one
        /// @param name Name of the object to be updated
        /// @param value The new value to SET
        template <class T>
        void SetValue(const std::string &name, T value)
        {
            // Get the object to be serialized
            Variable<T> *obj = static_cast<Variable<T> *>(ObjectProvider<IPrimitive>::Get(name));

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return;

            // If the old value is different from the new one, we save persistency values to memory, otherwise we do nothing to avoid unnecessary write operations to memory
            if (obj->SetValue(value))
            {
                std::string topic = MQTT_TOPIC_PREFIX;
                topic += name;

                std::string valueStr = VariableValueToString(value);

                executionEnv->GetMQTTManager()->Publish(topic.c_str(), valueStr.c_str(), true);

                executionEnv->GetBindingManager()->PropertyChanged(*obj, "value");

                writePersistencyData(name, obj);
            }
        }

        /// @brief Function that SET new variant value to the variable and save persistency values if the value is different from the previous one
        /// @param name Name of the object to be updated
        /// @param value The new variant value to SET
        void SetValue(const std::string &name, const VariableValue &value)
        {
            auto *base = ObjectProvider<IPrimitive>::Get(name);
            if (!base)
                return;

            auto *obj = base->AsVariableValue();
            if (!obj)
                return;

            if (obj->SetVariantValue(value))
            {
                std::string topic = MQTT_TOPIC_PREFIX;
                topic += name;

                std::string valueStr = VariableValueToString(value);

                executionEnv->GetMQTTManager()->Publish(topic.c_str(), valueStr.c_str(), true);

                executionEnv->GetBindingManager()->PropertyChanged(*obj, "value");

                if (auto *serializable = base->AsSerializable())
                    writePersistencyData(name, serializable);
            }
        }

#pragma region IPersistable
        void SavePersistencyValuesToStorage(const std::string &name) override
        {
            MM_LOG_TRACE("VariableProvider", "Saving persistency values for '%s' to storage", name.c_str());

            // Get the object to be serialized
            auto *base = ObjectProvider<IPrimitive>::Get(name);
            if (!base)
                return;

            auto *obj = base->AsSerializable();

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
            {
                MM_LOG_ERROR("VariableProvider", "Object is not serializable");
                return;
            }

            writePersistencyData(name, obj);
        }

        void GetPersistencyValuesFromStorage(const std::string &name, uint8_t *data, size_t length) override
        {
            MM_LOG_TRACE("VariableProvider", "Getting persistency values for '%s' from storage", name.c_str());

            uint16_t addr;
            executionEnv->GetMiniPrefs()->Get(name.c_str(), data, length, addr);
        }
#pragma endregion IPersistable

#pragma region IProviderDTO
        std::vector<DTOBase> GetDTOs() const override
        {
            std::vector<DTOBase> result;
            result.reserve(this->size());

            this->ForEach(
                [&](const std::string &name, IPrimitive *base)
                {
                    DTOBase dto;
                    if (dtoMapper && dtoMapper->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<IPrimitive>::Get(name);

            if (!obj)
                return false;

            if (dtoMapper && dtoMapper->ToDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

        /// @brief Function that fill the given DTO with the detailed data of the object, this will be used to expose object data through API in a generic way without needing to know the object type, we will just use the DTOBase fields to expose data, this function can be used to expose more detailed data than GetDTO function
        /// @param name Name of the object to get details
        /// @param dto Reference to the DTO that will receive the object data
        /// @return Return true if the DTO was filled successfully, false if there is an error during data retrieval
        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<IPrimitive>::Get(name);

            if (!obj)
                return false;

            if (dtoMapper && dtoMapper->ToDetailDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }
#pragma endregion IProviderDTO
    };
}