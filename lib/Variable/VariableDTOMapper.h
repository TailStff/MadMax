#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "Variable.h"

namespace MadMax
{
    class VariableDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsVariableValue();

            if (!primitive)
                return false;

            dto.fields.push_back({"value", primitive->GetVariantValue()});
            dto.objectName = name;

            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsVariableValue();

            if (!primitive)
                return false;

            if (ToDTO(obj, dto, name))
            {
                dto.fields.push_back({"type", GetDataType(primitive->GetVariantValue())});
                return true;
            }
            return false;
        }
    };
}