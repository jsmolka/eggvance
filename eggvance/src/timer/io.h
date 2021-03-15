#pragma once

#include "base/register.h"

class TimerChannel;

class TimerCount
{
public:
    TimerCount(TimerChannel& channel);

    u8 read(uint index);
    void write(uint index, u8 byte);

    u16 counter = 0;
    u16 initial = 0;

private:
    TimerChannel& channel;
};

class TimerControl : public XRegister<u16>
{
public:
    TimerControl(TimerChannel& channel);

    void write(uint index, u8 byte);

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enabled   = 0;

private:
    TimerChannel& channel;
};
