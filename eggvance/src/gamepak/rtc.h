#pragma once

#include "gamepak/gpio.h"

class Rtc : public Gpio
{
public:
    Rtc();

    void reset() final;

    u16 read(u32 addr) final;
    void write(u32 addr, u16 half) final;
};
