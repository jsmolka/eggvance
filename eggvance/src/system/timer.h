#pragma once

#include "registers/timerdata.h"
#include "registers/timercontrol.h"

class Timer
{
public:
    Timer(int id);

    void reset();

    void run(int cycles);
    void start();
    void update();

    int nextOverflow() const;

    int id;
    Timer* next;
    TimerData data;
    TimerControl control;

private:
    int reload;
    int counter;
    int overflow;
};
