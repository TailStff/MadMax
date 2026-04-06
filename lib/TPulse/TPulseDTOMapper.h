#include "IDTOMapperBase.h"
#include "TPulse.h"

namespace MadMax
{
    class TPulseDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const TPulse *primitive = static_cast<const TPulse *>(&obj);

            dto.fields.push_back({"value", primitive->GetValue()});
            dto.objectName = name;

            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const TPulse *primitive = static_cast<const TPulse *>(&obj);

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
}