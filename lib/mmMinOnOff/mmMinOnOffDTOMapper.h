#include "IDTOMapperBase.h"
#include "mmMinOnOff.h"

class mmMinOnOffDTOMapper : public IDTOMapperBase
{
public:
    bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmMinOnOff *>(&obj);

        dto.objectName = name;
        dto.fields.push_back({"value", primitive->GetValue()});
        return true;
    }

    bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmMinOnOff *>(&obj);

        if (ToDTO(obj, dto, name))
        {
            auto &statusObj = primitive->GetStatus();

            dto.fields.push_back({"input", statusObj.input});
            dto.fields.push_back({"minOnTime", statusObj.minOnTime});
            dto.fields.push_back({"minOffTime", statusObj.minOffTime});
            dto.fields.push_back({"remainingTime", statusObj.remainingTime});

            return true;
        }
        return false;
    }
};