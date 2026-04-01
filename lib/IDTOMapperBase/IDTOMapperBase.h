#pragma once

#include "ISerializableBase.h"
#include "IObjectDTO.h"

namespace MadMax
{
    class IDTOMapperBase
    {
    public:
        virtual bool ToDTO(const ISerializableBase &, DTOBase &, const std::string &name) const = 0;
        virtual bool ToDetailDTO(const ISerializableBase &, DTOBase &, const std::string &name) const = 0;
        virtual ~IDTOMapperBase() = default;
    };
}