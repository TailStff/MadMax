#include "ObjectProvider.h"
#include "mmPumpSwap.h"
#include "IPersistable.h"
#include "mmPumpSwapDTOMapper.h"

class mmPumpSwapProvider : public ObjectProvider<mmPumpSwap>, public IPersistable, public IProviderDTO
{
private:
    std::unique_ptr<IDTOMapperBase> dtoMappers;

public:
    mmPumpSwapProvider(ExecutionEnv *executionEnv)
    {
        this->executionEnv = executionEnv;
        dtoMappers = std::make_unique<mmPumpSwapDTOMapper>();
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

    mmPumpSwap *Create(const std::string &name, uint32_t address, uint8_t count, uint32_t feedbackDelay, mmPumpSwapPersistencyValues data = {})
    {
        // We resize our vector to be able to receive persistency values
        data.data.resize(count);

        uint16_t writtenLength;
        uint16_t addr;
        executionEnv->GetMiniPrefs()->Get(name.c_str(), reinterpret_cast<uint8_t *>(data.data.data()), mmPumpSwap::GetSerializedSize(count), addr, writtenLength);

        return ObjectProvider<mmPumpSwap>::Create(name, address, executionEnv, count, feedbackDelay, data);
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
            [&](const std::string &name, mmPumpSwap *base)
            {
                DTOBase dto;
                if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                    result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
            });

        return result;
    }

    bool GetDTO(const std::string &name, DTOBase &dto) const override
    {
        auto *obj = ObjectProvider<mmPumpSwap>::Get(name);

        if (!obj)
            return false;

        if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
            dto.objectName = name;

        return true;
    }

    bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
    {
        auto *obj = ObjectProvider<mmPumpSwap>::Get(name);

        if (!obj)
            return false;

        if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
            dto.objectName = name;

        return true;
    }

#pragma endregion IProviderDTO
};