#pragma once

#include "register.h"

class IRQEnabled : public Register<2>
{
public:
    operator int() const;

    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int enabled;
};
