#pragma once

#include "registers/timerdata.h"
#include "registers/timercontrol.h"

class Timer
{
public:
    Timer(uint id);

    void reset();

    void run(uint cycles);
    void start();
    void update();

    uint nextOverflow() const;

    uint id;
    Timer* next;
    TimerData data;
    TimerControl control;

private:
    uint prescale(uint value) const;

    uint counter;
    uint reload;
    uint overflow;
};
