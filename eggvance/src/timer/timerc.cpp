#include "timerc.h"

#include <algorithm>

#include "arm/arm.h"

TimerController::TimerController()
{
    timers[1].prev = &timers[0];
    timers[2].prev = &timers[1];
    timers[3].prev = &timers[2];

    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    for (auto& timer : timers)
    {
        timer.count.update_count = std::bind(&TimerController::runTimers, this);;
        timer.control.update_count = std::bind(&TimerController::runTimers, this);;

        timer.control.update_timer = [&](bool enabled)
        {
            if (enabled)
                timer.init();
            else
                timer.update();

            schedule();
        };
    }
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
    arm.state &= ~Arm::kStateTimer;

    event = kEventMax;
    for (auto& timer : timers)
    {
        if (timer.control.enable && !timer.control.cascade)
        {
            active.push_back(std::ref(timer));
            arm.state |= Arm::kStateTimer;

            event = std::min(event, timer.nextEvent());
        }
    }
}

void TimerController::reschedule()
{
    event = kEventMax;
    for (Timer& timer : active)
        event = std::min(event, timer.nextEvent());
}
