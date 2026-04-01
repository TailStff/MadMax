#include "ObjectProvider.h"
#include "RunTime.h"
#include "IPersistable.h"
#include "RunTimeDTOMapper.h"

namespace MadMax
{
    class RunTimeProvider : public ObjectProvider<RunTime>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

    public:
        RunTimeProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMappers = std::make_unique<RunTimeDTOMapper>();
        }

        RunTime *Create(const std::string &name, uint32_t address, RunTimePersistencyValues data = {.value = 0})
        {
            // Mini prefs method
            // executionEnv->GetMiniPrefs()->read(address, reinterpret_cast<uint8_t *>(&data), mmRunTime::GetSerializedSize());

            return ObjectProvider<RunTime>::Create(name, address, executionEnv, data);
        }

        void SavePersistencyValuesToMem(const std::string &name) override
        {
            int32_t address;

            // Get the object to be serialized
            auto *obj = this->Get(name, address);

            // If the object doesn't exist, we can't save its persistency values
            if (!obj)
                return;

            // Get the bytes vector that represent the object persistency values
            std::vector<uint8_t> dataToWrite;
            obj->GetBytesFromData(dataToWrite);

            executionEnv->GetMiniPrefs()->Put(name.c_str(), dataToWrite.data(), dataToWrite.size());
        }

        void GetPersistencyValuesFromMem(const std::string &name, uint8_t *data, size_t length) override
        {
            uint16_t readedLength;
            uint16_t addr;
            executionEnv->GetMiniPrefs()->Get(name.c_str(), data, length, addr, readedLength);
        }

#pragma region IProviderDTO
        std::vector<DTOBase> GetDTOs() const override
        {
            std::vector<DTOBase> result;
            result.reserve(this->size());

            this->ForEach(
                [&](const std::string &name, RunTime *base)
                {
                    DTOBase dto;
                    if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<RunTime>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<RunTime>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

#pragma endregion IProviderDTO
    };
}