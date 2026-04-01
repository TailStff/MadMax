// DTOBase.h
#pragma once

#include <string>
#include <vector>
#include "mmVariableValue.h"

namespace MadMax
{
    struct FieldValue
    {
        std::string key;
        mmVariableValue value;
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

    class IObjectDTO
    {
    public:
        virtual ~IObjectDTO() = default;
        virtual bool GetDTO(DTOBase &dto) const = 0;
        virtual bool GetDetailDTO(DTOBase &dto) const = 0;
    };
}