#include "ObjectProvider.h"
#include "mmLinear.h"

namespace MadMax
{
    class mmLinearProvider : public ObjectProvider<mmLinear>
    {
    private:
    public:
        mmLinearProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
        }

        mmLinear *Create(const std::string &name, uint32_t address)
        {
            return ObjectProvider<mmLinear>::Create(name, address, executionEnv);
        }
    };
}