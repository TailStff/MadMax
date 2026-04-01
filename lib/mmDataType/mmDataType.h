#ifndef MMDATATYPE_H
#define MMDATATYPE_H

namespace MadMax
{
    enum mmDataType
    {
        unknownType = 0,
        doubleType = 1,
        floatType = 2,
        int64Type = 3,
        uint64Type = 4,
        int32Type = 5,
        uint32Type = 6,
        int16Type = 7,
        uint16Type = 8,
        int8Type = 9,
        uint8Type = 10,
        boolType = 11,

        arrayOfUint8Type = 110,
        arrayOfBoolType = 111
    };
}

#endif