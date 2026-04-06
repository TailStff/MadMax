#pragma once

#include <string>
#include <vector>
#include "VariableValue.h"

namespace MadMax
{
    struct FieldValue
    {
        std::string key;
        VariableValue value;
    };

    struct DTOBase
    {
        std::string objectName;
        std::vector<FieldValue> fields;
        std::vector<DTOBase> children;
    };

    class IProviderDTO
    {
    public:
        virtual ~IProviderDTO() = default;
        virtual std::vector<DTOBase> GetDTOs() const = 0;
        virtual bool GetDTO(const std::string &name, DTOBase &dto) const = 0;
        virtual bool GetDetailDTO(const std::string &name, DTOBase &dto) const = 0;
    };


}