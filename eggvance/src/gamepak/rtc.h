#pragma once

#include "gpio.h"

class Rtc : public Gpio
{
public:
    Rtc();

    void reset() final;

protected:
    u16 readPort() final;
    void writePort(u16 half) final;
};
