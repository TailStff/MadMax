#pragma once

#include "ObjectProvider.h"
#include "IObjectDTO.h"
#include "Accum.h"
#include "AccumDTOMapper.h"

namespace MadMax
{
    class AccumProvider : public ObjectProvider<ISerializableBase>, public IPersistable, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

    public:
        AccumProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMappers = std::make_unique<AccumDTOMapper>();
        }

        template <class T>
        Accum<T> *Create(const std::string &name, int32_t address, AccumPersistencyValues<T> data = {.value = static_cast<T>(0)})
        {
            GetPersistencyValuesFromMem(name, reinterpret_cast<uint8_t *>(&data), sizeof(AccumPersistencyValues<T>));

            // mmAccum<T> varies per T, so we cannot use ObjectProvider<mmAccum<T>> as base.
            // We must inject via the fixed base interface ObjectProvider<ISerializableBase>
            // to store all typed instances in a single polymorphic collection.
            auto *obj = new Accum<T>(executionEnv, data);
            ObjectProvider<ISerializableBase>::inject(name, address, obj);
            return obj;
        }

        template <class T>
        Accum<T> *Get(const std::string &name)
        {
            int32_t address;
            return static_cast<Accum<T> *>(ObjectProvider<ISerializableBase>::Get(name, address));
        }

        void GetPersistencyValuesFromMem(const std::string &name, uint8_t *data, size_t length)
        {
            uint16_t readedLength;
            uint16_t addr;
            executionEnv->GetMiniPrefs()->Get(name.c_str(), data, length, addr, readedLength);
        }

        void SavePersistencyValuesToMem(const std::string &name) override
        {
            int32_t address;
            ISerializableBase *obj = ObjectProvider<ISerializableBase>::Get(name, address);
            if (!obj)
                return;

            std::vector<uint8_t> dataToWrite;
            obj->GetBytesFromData(dataToWrite);

            executionEnv->GetMiniPrefs()->Put(name.c_str(), dataToWrite.data(), dataToWrite.size());
        }

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