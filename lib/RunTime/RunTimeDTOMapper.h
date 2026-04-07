#pragma once

#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "Variable.h"

namespace MadMax
{
    class RunTimeDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsRunTime();

            if (!primitive)
                return false;

            dto.fields.push_back({"value", primitive->GetStatus().value});
            dto.objectName = name;

            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsRunTime();

            if (!primitive)
                return false;

            if (ToDTO(obj, dto, name))
            {
                dto.fields.push_back({"input", primitive->GetStatus().input});
                return true;
            }

            return false;
        }
    };
}