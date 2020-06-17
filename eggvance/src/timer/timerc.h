#pragma once

#include <array>
#include <functional>
#include <vector>

#include "timer.h"

class TimerController
{
public:
    friend class Io;

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

    void writeControl(Timer& timer, u8 byte);

    int counter = 0;
    int overflow = 0;

    std::array<Timer, 4> timers = { 0, 1, 2, 3 };
    std::vector<std::reference_wrapper<Timer>> active_timers;
};

extern TimerController timerc;
