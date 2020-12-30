#pragma once

#include <shell/buffer.h>

#include "constants.h"
#include "timerchannel.h"

class Timer
{
public:
    Timer();

    SHELL_INLINE void run(int cycles)
    {
        count += cycles;

        if (count >= event)
        {
            runChannels();
            reschedule();
        }
    }

    void runUntilIrq(int& cycles);

    TimerChannel channels[4] = { 0, 1, 2, 3 };

private:
    void schedule();
    void reschedule();
    void runChannels();

    uint count = 0;
    uint event = kEventMax;

    shell::FixedBuffer<TimerChannel*, 4> active;
};

inline Timer timer;
