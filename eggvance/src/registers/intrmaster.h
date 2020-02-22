#pragma once

#include "register.h"

class IntrMaster : public TRegister<IntrMaster, 2>
{
public:
    inline operator bool() const
    {
        return data[0] & 0x1;
    }
};
