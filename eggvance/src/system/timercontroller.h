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

    uint overflow;
    uint counter;

    Timer timers[4];
    SmallVector<Timer*, 4> active;
};

extern TimerController timerc;
