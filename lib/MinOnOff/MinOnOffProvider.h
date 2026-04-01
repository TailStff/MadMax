#include "ObjectProvider.h"
#include "MinOnOff.h"
#include "MinOnOffDTOMapper.h"

namespace MadMax
{
    class MinOnOffProvider : public ObjectProvider<MinOnOff>, public IProviderDTO
    {
    private:
        std::unique_ptr<IDTOMapperBase> dtoMappers;

    public:
        MinOnOffProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
            dtoMappers = std::make_unique<MinOnOffDTOMapper>();
        }

        MinOnOff *Create(const std::string &name, uint32_t address, bool initialValue = false)
        {
            return ObjectProvider<MinOnOff>::Create(name, address, executionEnv, initialValue);
        }

#pragma region IProviderDTO
        std::vector<DTOBase> GetDTOs() const override
        {
            std::vector<DTOBase> result;
            result.reserve(this->size());

            this->ForEach(
                [&](const std::string &name, MinOnOff *base)
                {
                    DTOBase dto;
                    if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                        result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
                });

            return result;
        }

        bool GetDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<MinOnOff>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

        bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
        {
            auto *obj = ObjectProvider<MinOnOff>::Get(name);

            if (!obj)
                return false;

            if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
                dto.objectName = name;

            return true;
        }

#pragma endregion IProviderDTO
    };
}