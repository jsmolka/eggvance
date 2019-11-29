#pragma once

#include "register.h"

class KeyControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int mask;
    int irq;
    int logic;
};
