#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "mmVariable.h"

class mmPumpSwapDTOMapper : public IDTOMapperBase
{
public:
    bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const mmPumpSwap *primitive = static_cast<const mmPumpSwap *>(&obj);

        dto.objectName = name;
        dto.fields.push_back({"values", primitive->GetPhysicalValues()});
        return true;
    }

    bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
    {
        const mmPumpSwap *primitive = static_cast<const mmPumpSwap *>(&obj);

        if (ToDTO(obj, dto, name))
        {

            for (auto i = 0; i < primitive->GetCount(); i++)
            {
                DTOBase status;
                status.objectName = std::to_string(i);

                auto &statusObj = primitive->GetStatus(i);

                status.fields.push_back({"command", statusObj.command});
                status.fields.push_back({"feedback", statusObj.feedback});
                status.fields.push_back({"value", statusObj.output});
                status.fields.push_back({"runTime", statusObj.runTimeValue});
                status.fields.push_back({"startCount", statusObj.startCountValue});
                status.fields.push_back({"fault", statusObj.fault});
                status.fields.push_back({"feedbackFault", statusObj.feedbackFault});

                dto.children.push_back(std::move(status));
            }

            return true;
        }
        return false;
    }
};