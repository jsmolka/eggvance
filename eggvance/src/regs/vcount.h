#pragma once

#include "register.h"

class VCount : public Register<2>
{
public:
    operator int() const;
    VCount& operator=(int value);

    void reset();

    u8 readByte(int index);
};
