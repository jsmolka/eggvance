#pragma once

#include "register.h"

class TimerData : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    u16 data;
    u16 initial;
};
