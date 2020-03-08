#pragma once

#include "timer.h"
#include "common/smallvector.h"

class TimerController
{
public:
    friend class IO;

    TimerController();

    void reset();

    void run(int cycles);
    void runUntilIrq(int& cycles);

private:
    void runTimers();
    void schedule();
    void reschedule();

    u8 read(u32 addr);
    void write(u32 addr, u8 byte);

    uint overflow = 0x7FFF'FFFF;
    uint counter  = 0;

    Timer timers[4] = { 0, 1, 2, 3 };
    SmallVector<Timer*, 4> active;
};

extern TimerController timerc;
