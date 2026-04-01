#ifndef HELPERSVECTOR_H
#define HELPERSVECTOR_H

#include <stdio.h>
#include <vector>

namespace HelpersVectors
{
    static size_t countTrue(std::initializer_list<bool> values)
    {
        size_t count = 0;

        const bool *p = values.begin();
        const bool *end = values.end();

        while (p != end)
            count += *p++;

        return count;
    }
}

#endif