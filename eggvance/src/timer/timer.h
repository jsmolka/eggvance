#pragma once

#include "timer/io.h"

class Timer
{
public:
    Timer(uint id)
        : id(id) {}

    void init();
    void update();
    void run(int cycles);

    uint nextEvent() const;

    uint id;
    Timer* prev = nullptr;
    Timer* next = nullptr;
    TimerCount count;
    TimerControl control;

private:
    uint counter  = 0;
    uint initial  = 0;
    uint overflow = 0;
};
