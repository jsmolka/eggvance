#include "timercontroller.h"

#include <algorithm>

#include "arm/arm.h"
#include "common/macros.h"
#include "mmu/memmap.h"
#include "registers/macros.h"

TimerController timerc;

TimerController::TimerController()
    : timers { 0, 1, 2, 3 }
{
    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    reset();
}

void TimerController::reset()
{
    active.clear();

    timers[0].reset();
    timers[1].reset();
    timers[2].reset();
    timers[3].reset();

    counter  = 0;
    overflow = 0x7FFF'FFFF;
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

#define READ_TIMER_DATA(label, data)    \
    CASE2(label):                       \
        runTimers();                    \
        return data.read(addr - label)  

u8 TimerController::read(u32 addr)
{
    switch (addr)
    {
    READ_TIMER_DATA(REG_TM0CNT_L, timers[0].data);
    READ_TIMER_DATA(REG_TM1CNT_L, timers[1].data);
    READ_TIMER_DATA(REG_TM2CNT_L, timers[2].data);
    READ_TIMER_DATA(REG_TM3CNT_L, timers[3].data);

    READ1(REG_TM0CNT_H, timers[0].control);
    READ1(REG_TM1CNT_H, timers[1].control);
    READ1(REG_TM2CNT_H, timers[2].control);
    READ1(REG_TM3CNT_H, timers[3].control);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

#define WRITE_TIMER_CONTROL(label, timer)       \
    case label:                                 \
    {                                           \
        runTimers();                            \
        int enabled = timer.control.enabled;    \
        timer.control.write(0, byte);           \
        if (!enabled && timer.control.enabled)  \
            timer.start();                      \
        else if (enabled)                       \
            timer.update();                     \
        schedule();                             \
        break;                                  \
    }

void TimerController::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE2(REG_TM0CNT_L, timers[0].data);
    WRITE2(REG_TM1CNT_L, timers[1].data);
    WRITE2(REG_TM2CNT_L, timers[2].data);
    WRITE2(REG_TM3CNT_L, timers[3].data);

    WRITE_TIMER_CONTROL(REG_TM0CNT_H, timers[0]);
    WRITE_TIMER_CONTROL(REG_TM1CNT_H, timers[1]);
    WRITE_TIMER_CONTROL(REG_TM2CNT_H, timers[2]);
    WRITE_TIMER_CONTROL(REG_TM3CNT_H, timers[3]);

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void TimerController::runTimers()
{
    for (auto& timer : active)
    {
        timer->run(counter);
    }
    overflow -= counter;
    counter = 0;
}

void TimerController::schedule()
{
    active.clear();

    overflow = 0x7FFF'FFFF;

    for (auto& timer : timers)
    {
        if (timer.control.enabled && !timer.control.cascade)
        {
            active.push_back(&timer);

            overflow = std::min(overflow, timer.nextOverflow());
        }
    }

    if (active.size() > 0)
        arm.state |= ARM::STATE_TIMER;
    else
        arm.state &= ~ARM::STATE_TIMER;
}

void TimerController::reschedule()
{
    overflow = 0x7FFF'FFFF;

    for (auto& timer : active)
    {
        overflow = std::min(overflow, timer->nextOverflow());
    }
}
