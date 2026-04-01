#include "ObjectProvider.h"
#include "PID.h"

namespace MadMax
{
    class PIDProvider : public ObjectProvider<PID>
    {
    private:
    public:
        PIDProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
        }

        PID *Create(const std::string &name, uint32_t address)
        {
            return ObjectProvider<PID>::Create(name, address, executionEnv);
        }
    };
}