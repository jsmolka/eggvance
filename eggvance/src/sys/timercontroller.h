#pragma once

#include <vector>

#include "timer.h"

class TimerController
{
public:
    TimerController();

    void reset();

    void run(u64 cycles);
    void runUntil(int& cycles);

    u8 readByte(u32 addr);
    void writeByte(u32 addr, u8 byte);

private:
    void update();
    void rebuild();

    u64 threshold;
    u64 counter;
    u64 last;

    Timer timers[4] = { 0, 1, 2, 3 };

    std::vector<Timer*> active;
};
