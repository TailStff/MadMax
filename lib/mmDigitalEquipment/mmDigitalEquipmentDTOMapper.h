#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "mmVariable.h"

class mmDigitalEquipmentDTOMapper : public IDTOMapperBase
{
public:
    bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmDigitalEquipment *>(&obj);

        dto.objectName = name;
        dto.fields.push_back({"value", primitive->GetValue()});
        return true;
    }

    bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const auto *primitive = static_cast<const mmDigitalEquipment *>(&obj);

        if (ToDTO(obj, dto, name))
        {
            auto &statusObj = primitive->GetStatus();

            dto.fields.push_back({"command", statusObj.command});
            dto.fields.push_back({"feedback", statusObj.feedback});
            dto.fields.push_back({"value", statusObj.output});
            dto.fields.push_back({"runTime", statusObj.runTimeValue});
            dto.fields.push_back({"startCount", statusObj.startCountValue});
            dto.fields.push_back({"fault", statusObj.fault});
            dto.fields.push_back({"feedbackFault", statusObj.feedbackFault});

            return true;
        }
        return false;
    }
};