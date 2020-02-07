#pragma once

#include <vector>

#include "timer.h"

class TimerController
{
public:
    TimerController();

    void reset();

    void run(int cycles);
    void runUntil(int& cycles);

    u8 readByte(u32 addr);
    void writeByte(u32 addr, u8 byte);

    std::vector<Timer*> active;  // Todo: private

private:
    void runTimers();
    void schedule();
    void reschedule();

    uint overflow;
    uint counter;

    Timer timers[4];
};

extern TimerController timerc;
