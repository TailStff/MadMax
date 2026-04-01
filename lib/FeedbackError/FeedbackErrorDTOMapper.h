#include "IDTOMapperBase.h"
#include "FeedbackError.h"

namespace MadMax
{
    class FeedbackErrorDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = static_cast<const FeedbackError *>(&obj);

            dto.objectName = name;
            dto.fields.push_back({"value", primitive->GetValue()});
            return true;
        }

        bool ToDetailDTO(const ISerializableBase &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = static_cast<const FeedbackError *>(&obj);

            if (ToDTO(obj, dto, name))
            {
                auto &statusObj = primitive->GetStatus();

                dto.fields.push_back({"command", statusObj.command});
                dto.fields.push_back({"feedback", statusObj.feedback});
                dto.fields.push_back({"offDelayFeedbackError", statusObj.offDelayFeedbackError});
                dto.fields.push_back({"onDelayFeedbackError", statusObj.onDelayFeedbackError});
                dto.fields.push_back({"option", (int)statusObj.option});
                dto.fields.push_back({"remainingTime", statusObj.remainingTime});
                dto.fields.push_back({"state", (int)statusObj.state});

                return true;
            }
            return false;
        }
    };
}