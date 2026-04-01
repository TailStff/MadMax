#include "IDTOMapperBase.h"
#include "mmDelayOnOff.h"

class mmDelayOnOffDTOMapper : public IDTOMapperBase
{
public:
    bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmDelayOnOff *>(&obj);

        dto.objectName = name;
        dto.fields.push_back({"value", primitive->GetValue()});
        return true;
    }

    bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmDelayOnOff *>(&obj);

        if (ToDTO(obj, dto, name))
        {
            auto &statusObj = primitive->GetStatus();

            dto.fields.push_back({"input", statusObj.input});
            dto.fields.push_back({"delayOn", statusObj.delayOn});
            dto.fields.push_back({"delayOff", statusObj.delayOff});
            dto.fields.push_back({"remainingTime", statusObj.remainingTime});

            return true;
        }
        return false;
    }
};