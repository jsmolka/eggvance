#pragma once

#include "registers/timercontrol.h"

class Timer
{
public:
    Timer(int id, TimerControl& control, Timer* next);

    void reset();
    void emulate(int cycles);

private:
    int id;
    Timer* next;
    TimerControl& control;

    void cascade();
    void interrupt();
    void increment(int amount);
};
