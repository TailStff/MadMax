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

        PID *Create(const std::string &name)
        {
            return ObjectProvider<PID>::Create(name, executionEnv);
        }
    };
}