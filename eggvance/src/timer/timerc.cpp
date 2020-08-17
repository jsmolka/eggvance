#include "timerc.h"

#include <algorithm>

#include "core/core.h"

TimerController::TimerController(Core& core)
    : core(core)
    , timers{ Timer{ core, 1 }, { core, 2 }, { core, 3 }, { core, 4 } }
{
    timers[1].prev = &timers[0];
    timers[2].prev = &timers[1];
    timers[3].prev = &timers[2];

    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    event = 1 << 30;
}

void TimerController::run(int cycles)
{
    count += cycles;

    if (count >= event)
    {
        runTimers();
        reschedule();
    }
}

void TimerController::runUntilIrq(int& cycles)
{
    int remaining = event - count;
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
        timer.run(count);

    event -= count;
    count = 0;
}

void TimerController::schedule()
{
    active.clear();
    core.arm.state &= ~ARM::kStateTimer;

    event = 1 << 30;
    for (auto& timer : timers)
    {
        if (timer.io.control.enable && !timer.io.control.cascade)
        {
            active.push_back(std::ref(timer));
            core.arm.state |= ARM::kStateTimer;

            event = std::min(event, timer.nextEvent());
        }
    }
}

void TimerController::reschedule()
{
    event = 1 << 30;
    for (Timer& timer : active)
        event = std::min(event, timer.nextEvent());
}
