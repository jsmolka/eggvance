#pragma once

#include <vector>

#include "constants.h"
#include "timerchannel.h"

class Timer
{
public:
    Timer();

    void run(int cycles);
    void runUntilIrq(int& cycles);

    TimerChannel channels[4] = { 0, 1, 2, 3 };

private:
    void schedule();
    void reschedule();
    void runChannels();

    uint count = 0;
    uint event = kEventMax;

    std::vector<std::reference_wrapper<TimerChannel>> active;
};

inline Timer timer;
