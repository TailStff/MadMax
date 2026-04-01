#include "ObjectProvider.h"
#include "mmFeedbackError.h"
#include "mmFeedbackErrorDTOMapper.h"

class mmFeedbackErrorProvider : public ObjectProvider<mmFeedbackError>, public IProviderDTO
{
private:
    std::unique_ptr<IDTOMapperBase> dtoMappers;

public:
    mmFeedbackErrorProvider(ExecutionEnv *executionEnv)
    {
        this->executionEnv = executionEnv;
        dtoMappers = std::make_unique<mmFeedbackErrorDTOMapper>();
    }

    mmFeedbackError *Create(const std::string &name, uint32_t address)
    {
        return ObjectProvider<mmFeedbackError>::Create(name, address, executionEnv);
    }

#pragma region IProviderDTO
    std::vector<DTOBase> GetDTOs() const override
    {
        std::vector<DTOBase> result;
        result.reserve(this->size());

        this->ForEach(
            [&](const std::string &name, mmFeedbackError *base)
            {
                DTOBase dto;
                if (dtoMappers && dtoMappers->ToDTO(*base, dto, name))
                    result.emplace_back(std::move(dto)); // like result.push_back(dto) but faster
            });

        return result;
    }

    bool GetDTO(const std::string &name, DTOBase &dto) const override
    {
        auto *obj = ObjectProvider<mmFeedbackError>::Get(name);

        if (!obj)
            return false;

        if (dtoMappers && dtoMappers->ToDTO(*obj, dto, name))
            dto.objectName = name;

        return true;
    }

    bool GetDetailDTO(const std::string &name, DTOBase &dto) const override
    {
        auto *obj = ObjectProvider<mmFeedbackError>::Get(name);

        if (!obj)
            return false;

        if (dtoMappers && dtoMappers->ToDetailDTO(*obj, dto, name))
            dto.objectName = name;

        return true;
    }

#pragma endregion IProviderDTO
};