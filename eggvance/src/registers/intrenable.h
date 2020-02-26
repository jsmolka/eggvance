#pragma once

#include "register.h"

class IntrEnable : public Register<IntrEnable, 2>
{
public:
    inline operator u16()
    {
        return *reinterpret_cast<u16*>(data);
    }
};
