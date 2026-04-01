#include "IDTOMapperBase.h"
#include "mmTPulse.h"

class mmTPulseDTOMapper : public IDTOMapperBase
{
public:
    bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const mmTPulse *primitive = static_cast<const mmTPulse *>(&obj);

        dto.fields.push_back({"value", primitive->GetValue()});
        dto.objectName = name;

        return true;
    }

    bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const mmTPulse *primitive = static_cast<const mmTPulse *>(&obj);

        if (ToDTO(obj, dto, name))
        {
            auto status = primitive->GetStatus();

            dto.fields.push_back({"input", status.input});
            dto.fields.push_back({"delay", status.delay});
            return true;
        }

        return false;
    }
};