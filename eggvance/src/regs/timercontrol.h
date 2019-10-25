#pragma once

#include "register.h"

class TimerControl : public Register<4>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int prescaler;
    int cascade;
    int irq;
    int enabled;
    u16 data;
    u16 initial;
};
