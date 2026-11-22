#include "IDTOMapperBase.h"
#include "IVariableValue.h"
#include "Variable.h"

namespace MadMax
{
    class PumpSwapDTOMapper : public IDTOMapperBase
    {
    public:
        bool ToDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsPumpSwap();

            if (!primitive)
                return false;

            dto.objectName = name;

            dto.fields.push_back({"values", primitive->GetPhysicalValues()});
            dto.actions.push_back({"Reset"});
            dto.actions.push_back({"Acknowledge"});
            return true;
        }

        bool ToDetailDTO(const IPrimitive &obj, DTOBase &dto, const std::string &name) const override
        {
            const auto *primitive = obj.AsPumpSwap();

            if (!primitive)
                return false;

            if (ToDTO(obj, dto, name))
            {
                auto pumpSwapResult = primitive->GetPumpSwapResult();
                dto.fields.push_back({"availablePumps", pumpSwapResult.AvailablePumps});
                dto.fields.push_back({"capacityState", (int)pumpSwapResult.CapacityState});
                dto.fields.push_back({"requestedPumps", pumpSwapResult.RequestedPumps});
                dto.fields.push_back({"runningPumps", pumpSwapResult.RunningPumps});
                dto.fields.push_back({"totalPumps", pumpSwapResult.TotalPumps});

                for (auto i = 0; i < pumpSwapResult.TotalPumps; i++)
                {
                    DTOBase status;
                    status.objectName = std::to_string(i);

                    status.fields.push_back({"command", pumpSwapResult.PumpsStatus[i]->command});
                    status.fields.push_back({"feedback", pumpSwapResult.PumpsStatus[i]->feedback});
                    status.fields.push_back({"value", pumpSwapResult.PumpsStatus[i]->output});
                    status.fields.push_back({"runTime", pumpSwapResult.PumpsStatus[i]->runTimeValue});
                    status.fields.push_back({"startCount", pumpSwapResult.PumpsStatus[i]->startCountValue});
                    status.fields.push_back({"fault", pumpSwapResult.PumpsStatus[i]->fault});
                    status.fields.push_back({"feedbackFault", pumpSwapResult.PumpsStatus[i]->feedbackFault});

                    dto.children.push_back(std::move(status));
                }

                return true;
            }
            return false;
        }
    };
}