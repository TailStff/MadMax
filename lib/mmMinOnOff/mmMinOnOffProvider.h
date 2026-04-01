#include "ObjectProvider.h"
#include "mmMinOnOff.h"

class mmMinOnOffProvider : public ObjectProvider<mmMinOnOff>
{
private:

public:
    mmMinOnOffProvider(ExecutionEnv *executionEnv)
    {
        this->executionEnv = executionEnv;
    }

    mmMinOnOff* Create(const std::string& name, uint32_t address, bool initialValue = false)
    {
        return ObjectProvider<mmMinOnOff>::Create(name, address, executionEnv, initialValue);
    }
};