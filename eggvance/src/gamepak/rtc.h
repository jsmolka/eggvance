#pragma once

#include "gpio.h"

class Rtc : public Gpio
{
public:
    Rtc();

    void reset() final;
};
