#pragma once

#include "io/timer_io.h"

class Timer
{
public:
    Timer(uint id);

    void run(uint cycles);
    void start();
    void update();

    uint nextOverflow() const;

    uint id;
    Timer* next = nullptr;
    TimerIO io;

private:
    uint prescale(uint value) const;

    uint counter  = 0;
    uint reload   = 0;
    uint overflow = 0;
};
