#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "Variable.h"

namespace MadMax
{
    class DigitalEquipmentDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsDigitalEquipment();

            if (!primitive)
                return false;

            dto.objectName = name;
            dto.fields.push_back({"value", primitive->GetValue()});
            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsDigitalEquipment();

            if (!primitive)
                return false;

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
}