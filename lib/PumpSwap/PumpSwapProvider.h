#pragma once

#include "ObjectProvider.h"
#include "PumpSwap.h"
#include "IPersistable.h"
#include "PumpSwapDTOMapper.h"

#include "mmLogger.h"

namespace MadMax
{
    class PumpSwapProvider : public ObjectProvider<PumpSwap>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

    public:
        PumpSwapProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMappers = std::make_unique<PumpSwapDTOMapper>();
        }

        void printByteBuffer(const uint8_t *buffer, size_t length)
        {
            Serial.print("[");
            for (size_t i = 0; i < length; i++)
            {
                if (i > 0)
                    Serial.print(", ");
                if (buffer[i] < 0x10)
                    Serial.print("0"); // pour avoir toujours deux chiffres
                Serial.print(buffer[i], HEX);
            }
            Serial.println("]");
        }

        PumpSwap *Create(const std::string &name, uint8_t count, uint32_t feedbackDelay, PumpSwapPersistencyValues data = {})
        {
            // We resize our vector to be able to receive persistency values
            data.data.resize(count);

            // Reset all persistency values to 0 before getting real values from memory, to avoid any issue with unserialization if the data in memory is corrupted (for example, if the count of pumps is changed and we have more pumps than before, we will have more persistency values to unserialize than before, so we need to be sure that all those new persistency values are initialized to 0 before unserialization to avoid any issue with unserialization of thoses new values)
            for (auto &entry : data.data)
            {
                entry.runTime = 0;
                entry.startCount = 0;
            }

            // Get values from memory to be able to create the object with his previous persistency values
            GetPersistencyValuesFromStorage(name, reinterpret_cast<uint8_t *>(data.data.data()), PumpSwap::GetSerializedSize(count));

            return ObjectProvider<PumpSwap>::Create(name, executionEnv, count, feedbackDelay, data);
        }

#pragma region IPersistable
        void SavePersistencyValuesToStorage(const std::string &name) override
        {
            MM_LOG_TRACE("PumpSwapProvider", "Saving persistency values for '%s' to storage", name.c_str());

            // Get the object to be serialized
            auto *obj = this->Get(name);

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return;

            // Get the bytes vector that represent the object persistency values
            std::vector<uint8_t> dataToWrite;
            obj->GetBytesFromData(dataToWrite);

            executionEnv->GetMiniPrefs()->Put(name.c_str(), dataToWrite.data(), dataToWrite.size());
        }

        void GetPersistencyValuesFromStorage(const std::string &name, uint8_t *data, size_t length) override
        {
            MM_LOG_TRACE("PumpSwapProvider", "Getting persistency values for '%s' from storage", name.c_str());

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
                [&](const std::string &name, PumpSwap *base)
                {
                    DTOBase dto;
                    if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<PumpSwap>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<PumpSwap>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

        bool TriggerAction(const std::string &name, const std::string &action, DTOBase &dto) const
        {
            auto *obj = ObjectProvider<PumpSwap>::Get(name);

            if (!obj)
                return false;

            if (action == "Reset")
            {
                obj->TriggerResetAction();

                // After triggering action, we update dto with new values to be able to return them in the response of the API call
                return GetDetailDTO(name, dto);
            }
            else if (action == "Acknowledge")
            {
                obj->TriggerAcknowledge();

                // After triggering action, we update dto with new values to be able to return them in the response of the API call
                return GetDetailDTO(name, dto);
            }

            return false;
        }

#pragma endregion IProviderDTO
    };
}