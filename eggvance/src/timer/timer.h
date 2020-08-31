#pragma once

#include <array>
#include <vector>

#include "timer/constants.h"
#include "timer/timerchannel.h"

class Timer
{
public:
    friend class Io;

    Timer();

    void run(int cycles);
    void runUntilIrq(int& cycles);

private:
    void schedule();
    void reschedule();
    void runChannels();

    uint count = 0;
    uint event = kEventMax;

    std::array<TimerChannel, 4> channels = { 0, 1, 2, 3 };
    std::vector<std::reference_wrapper<TimerChannel>> active;
};

inline Timer timer;
