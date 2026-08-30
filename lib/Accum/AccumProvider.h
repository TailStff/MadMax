#pragma once

#include "ObjectProvider.h"
#include "IObjectDTO.h"
#include "Accum.h"
#include "AccumDTOMapper.h"
#include "ModbusBinding.h"

#include "mmLogger.h"

#define MQTT_TOPIC_PREFIX "MadMax2/instance/Accums/"

namespace MadMax
{
    class AccumProvider : public ObjectProvider<IPrimitive>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

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
        AccumProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMappers = std::make_unique<AccumDTOMapper>();
        }

        template <class T>
        Accum<T> *Create(const std::string &name, AccumPersistencyValues<T> data = {.value = static_cast<T>(0)})
        {
            GetPersistencyValuesFromStorage(name, reinterpret_cast<uint8_t *>(&data), sizeof(AccumPersistencyValues<T>));

            // WriteValueToModbusSpace(address, data);

            // mmAccum<T> varies per T, so we cannot use ObjectProvider<mmAccum<T>> as base.
            // We must inject via the fixed base interface ObjectProvider<ISerializableBase>
            // to store all typed instances in a single polymorphic collection.
            auto *obj = new Accum<T>(executionEnv, data);
            ObjectProvider<IPrimitive>::inject(name, obj);

            // modbusMapper && modbusMapper->ExposeToModbus(*obj, address);

            return obj;
        }

        template <class T>
        Accum<T> *Get(const std::string &name)
        {
            return static_cast<Accum<T> *>(ObjectProvider<IPrimitive>::Get(name));
        }

        /// @brief Function that SET new value to the accum output value and save persistency values if the value is different from the previous one
        /// @param name Name of the object to be updated
        /// @param value The new value to SET
        template <class T>
        void SetValue(const std::string &name, T value)
        {
            // Get the object to be serialized
            Accum<T> *obj = static_cast<Accum<T> *>(ObjectProvider<IPrimitive>::Get(name));

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

        /// @brief Function that increment internal value by increment value when input value has rising edge
        /// @tparam T Any types
        /// @param name Name of the object to be updated
        /// @param input The Input value to be process
        /// @param increment The value to be added to the internal value at each rising edge of the input, if the increment is negative and the absolute value of the increment is greater than the current internal value, the internal value will be set to 0 to avoid underflow. If the increment is positive and the internal value is greater than the maximum value of T minus the increment, the internal value will be set to the maximum value of T to avoid overflow.
        /// @param resetTrigger The Reset trigger value, when this value has rising edge, the internal value will be reset to resetValue
        /// @param resetValue The value to reset the internal value when reset trigger has rising edge
        /// @return True if the internal value was updated, false otherwise
        template <class T>
        T Evaluate(const std::string &name, bool input, T increment, bool resetTrigger = false, T resetValue = static_cast<T>(0))
        {
            // Get the object to be serialized
            Accum<T> *obj = static_cast<Accum<T> *>(ObjectProvider<IPrimitive>::Get(name));

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return static_cast<T>(0);

            if (obj->Evaluate(input, increment, resetTrigger, resetValue))
            {
                std::string topic = MQTT_TOPIC_PREFIX;
                topic += name;

                std::string valueStr = VariableValueToString(obj->GetVariantValue());

                executionEnv->GetMQTTManager()->Publish(topic.c_str(), valueStr.c_str(), true);

                executionEnv->GetBindingManager()->PropertyChanged(*obj, "value");

                if (auto *serializable = obj->AsSerializable())
                    writePersistencyData(name, serializable);
            }

            return obj->GetValue();
        }

#pragma region IPersistable
        void SavePersistencyValuesToStorage(const std::string &name) override
        {
            MM_LOG_TRACE("AccumProvider", "Saving persistency values for '%s' to storage", name.c_str());

            // Get the object to be serialized
            auto *base = ObjectProvider<IPrimitive>::Get(name);
            if (!base)
                return;

            auto *obj = base->AsSerializable();

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
            {
                MM_LOG_ERROR("AccumProvider", "Object is not serializable");
                return;
            }

            writePersistencyData(name, obj);
        }

        void GetPersistencyValuesFromStorage(const std::string &name, uint8_t *data, size_t length) override
        {
            MM_LOG_TRACE("AccumProvider", "Getting persistency values for '%s' from storage", name.c_str());

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
                    if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            IPrimitive *obj = ObjectProvider<IPrimitive>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
            {
                dto.objectName = name;
            }

            return true;
        }

        /// @brief Function that fill the given DTO with the detailed data of the object, this will be used to expose object data through API in a generic way without needing to know the object type, we will just use the DTOBase fields to expose data, this function can be used to expose more detailed data than GetDTO function
        /// @param name Name of the object to get details
        /// @param dto Reference to the DTO that will receive the object data
        /// @return Return true if the DTO was filled successfully, false if there is an error during data retrieval
        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            IPrimitive *obj = ObjectProvider<IPrimitive>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
            {
                dto.objectName = name;
            }

            return true;
        }
#pragma endregion IProviderDTO
    };
}