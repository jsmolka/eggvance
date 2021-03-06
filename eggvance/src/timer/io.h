#pragma once

#include "base/register.h"

class TimerChannel;

class TimerRegister
{
public:
    TimerRegister(TimerChannel& channel);

protected:
    void run();

    TimerChannel& channel;
};

class TimerCount : public TimerRegister, public XRegister<u16>
{
public:
    using TimerRegister::TimerRegister;

    u8 read(uint index);
    void write(uint index, u8 byte);

    u16 initial = 0;
};

class TimerControl : public TimerRegister, public XRegister<u16, 0x00C7>
{
public:
    using TimerRegister::TimerRegister;

    void write(uint index, u8 byte);

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enabled   = 0;
};
