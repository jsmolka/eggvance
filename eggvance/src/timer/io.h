#pragma once

#include <functional>

#include "base/register.h"

class TimerCount : public XRegister<u16>
{
public:
    u8 read(uint index) const;
    void write(uint index, u8 byte);

    u16 initial = 0;

    std::function<void(void)> run_channels;
};

class TimerControl : public XRegister<u16>
{
public:
    TimerControl();

    void write(uint index, u8 byte);

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enable    = 0;

    std::function<void(bool)> on_write;
    std::function<void(void)> run_channels;
};
