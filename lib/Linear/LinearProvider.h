#include "ObjectProvider.h"
#include "Linear.h"

namespace MadMax
{
    class LinearProvider : public ObjectProvider<Linear>
    {
    private:
    public:
        LinearProvider(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
        }

        Linear *Create(const std::string &name)
        {
            return ObjectProvider<Linear>::Create(name, executionEnv);
        }
    };
}