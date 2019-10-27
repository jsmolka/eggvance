#pragma once

#include "regs/timerdata.h"
#include "regs/timercontrol.h"

class Timer
{
public:
    Timer(int id);

    void reset();
    void init();
    void run(int cycles);

    int nextOverflow() const;

    int id;
    Timer* next;

    TimerData data;
    TimerControl control;

private:
    void updateReload();

    int reload;
    int counter;
    int overflow;
};
