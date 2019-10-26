#pragma once

#include "register.h"

class TimerControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int prescaler;
    int cascade;
    int irq;
    int enabled;
};
