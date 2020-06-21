#include "timerc.h"

#include <algorithm>

#include "arm/arm.h"

TimerController timerc;

TimerController::TimerController()
{
    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    overflow = 1 << 30;
}

void TimerController::run(int cycles)
{
    counter += cycles;

    if (counter >= overflow)
    {
        runTimers();
        reschedule();
    }
}

void TimerController::runUntilIrq(int& cycles)
{
    int remaining = overflow - counter;
    if (remaining < cycles)
    {
        run(remaining);
        cycles -= remaining;
    }
    else
    {
        run(cycles);
        cycles = 0;
    }
}

void TimerController::runTimers()
{
    for (Timer& timer : active)
        timer.run(counter);

    overflow -= counter;
    counter = 0;
}

void TimerController::schedule()
{
    active.clear();
    arm.state &= ~ARM::kStateTimer;

    overflow = 1 << 30;
    for (auto& timer : timers)
    {
        if (timer.io.control.enable && !timer.io.control.cascade)
        {
            active.push_back(std::ref(timer));
            arm.state |= ARM::kStateTimer;

            overflow = std::min(overflow, timer.nextOverflow());
        }
    }
}

void TimerController::reschedule()
{
    overflow = 1 << 30;
    for (Timer& timer : active)
        overflow = std::min(overflow, timer.nextOverflow());
}
