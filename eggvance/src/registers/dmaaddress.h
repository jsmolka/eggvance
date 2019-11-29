#pragma once

#include "register.h"

class DMAAddress : public Register<4>
{
public:
    void reset();

    void write(int index, u8 byte);

    u32 addr;
};
