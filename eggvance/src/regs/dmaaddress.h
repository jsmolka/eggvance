#pragma once

#include "register.h"

class DMAAddress : public Register<4>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    u32 addr;
    u32 addr_mask;
};
