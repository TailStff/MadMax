#pragma once

#include "IPrimitive.h"
#include "IObjectDTO.h"

namespace MadMax
{
    class IDTOMapperBase
    {
    public:
        virtual bool ToDTO(const IPrimitive &, DTOBase &, const std::string &name) const = 0;
        virtual bool ToDetailDTO(const IPrimitive &, DTOBase &, const std::string &name) const = 0;
        virtual ~IDTOMapperBase() = default;
    };
}