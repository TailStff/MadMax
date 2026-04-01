#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "mmVariable.h"

namespace MadMax
{
    class mmVariableDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
        {
            const IVariableValue *primitive = static_cast<const IVariableValue *>(&obj);

            dto.fields.push_back({"value", primitive->GetVariantValue()});
            dto.objectName = name;

            return true;
        }

        bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
        {
            const IVariableValue *primitive = static_cast<const IVariableValue *>(&obj);

            if (ToDTO(obj, dto, name))
            {
                dto.fields.push_back({"type", GetDataType(primitive->GetVariantValue())});
                return true;
            }
            return false;
        }
    };
}