#pragma once

#include "register.h"

class TimerData : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    u16 data;
    u16 initial;
};
