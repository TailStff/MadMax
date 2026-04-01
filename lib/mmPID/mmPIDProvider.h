#include "ObjectProvider.h"
#include "mmPID.h"

class mmPIDProvider : public ObjectProvider<mmPID>
{
private:

public:
    mmPIDProvider(ExecutionEnv *executionEnv)
    {
        this->executionEnv = executionEnv;
    }

    mmPID* Create(const std::string& name, uint32_t address)
    {
        return ObjectProvider<mmPID>::Create(name, address, executionEnv);
    }
};