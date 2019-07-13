#pragma once

#include "registers/other/timercontrol.h"
#include "registers/other/timerdata.h"

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
