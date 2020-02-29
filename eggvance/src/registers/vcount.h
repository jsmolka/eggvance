#pragma once

#include "register.h"

class VCount : public RegisterR<2>
{
public:
    inline operator uint() const
    {
        return data[0];
    }

    inline void next()
    {
        data[0] = (data[0] + 1) % 228;
    }
};
