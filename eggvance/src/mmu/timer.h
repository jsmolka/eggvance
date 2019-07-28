#pragma once

#include "registers/timercontrol.h"
#include "registers/timerdata.h"

class Timer
{
public:
    Timer(TimerControl& control, TimerData& data);
    
    void init();
    void step();

    bool requestInterrupt() const;

    Timer* prev = nullptr;
    Timer* next = nullptr;
    int initial = 0;

private:
    void increment();
    bool overflowed() const;

    int counter = 0;
    TimerControl& control;
    TimerData& data;
};
