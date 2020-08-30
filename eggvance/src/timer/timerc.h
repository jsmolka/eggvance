#pragma once

#include <array>
#include <functional>
#include <vector>

#include "timer/constants.h"
#include "timer/timer.h"

class TimerController
{
public:
    friend class Io;

    TimerController();

    void run(int cycles);
    void runUntilIrq(int& cycles);

private:
    void runTimers();
    void schedule();
    void reschedule();

    uint count = 0;
    uint event = kEventMax;

    std::array<Timer, 4> timers = { 1, 2, 3, 4 };
    std::vector<std::reference_wrapper<Timer>> active;
};

inline TimerController timerc;
