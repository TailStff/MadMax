#include "IDTOMapperBase.h"
#include "MinOnOff.h"

namespace MadMax
{
    class MinOnOffDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsMinOnOff();

            if (!primitive)
                return false;

            dto.objectName = name;
            dto.fields.push_back({"value", primitive->GetValue()});
            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsMinOnOff();

            if (!primitive)
                return false;

            if (ToDTO(obj, dto, name))
            {
                auto &status = primitive->GetStatus();

                dto.fields.push_back({"input", status.input});
                dto.fields.push_back({"minOnTime", status.minOnTime});
                dto.fields.push_back({"minOffTime", status.minOffTime});
                dto.fields.push_back({"remainingTime", status.remainingTime});

                return true;
            }
            return false;
        }
    };
}