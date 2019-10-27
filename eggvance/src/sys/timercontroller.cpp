#include "timercontroller.h"

// Todo: REMOVE???!?!?
#include <algorithm>
#include <fmt/printf.h>

#include "common/macros.h"
#include "mmu/memmap.h"

#define CASE2(label) case label + 0: case label + 1:
#define CASE4(label) case label + 0: case label + 1: case label + 2: case label + 3:

#define READ2(label, reg) CASE2(label) return reg.read(addr - label)
#define READ4(label, reg) CASE4(label) return reg.read(addr - label)

#define WRITE2(label, reg) CASE2(label) reg.write(addr - label, byte); break
#define WRITE4(label, reg) CASE4(label) reg.write(addr - label, byte); break

TimerController::TimerController()
{
    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];
}

void TimerController::reset()
{
    active.clear();

    for (auto& timer : timers)
    {
        timer.reset();
    }

    counter  = 0;
    overflow = 0x3FFF'FFFF;
}

void TimerController::run(int cycles)
{
    counter += cycles;

    if (counter >= overflow)
    {
        runTimers();
        schedule();
    }
}

void TimerController::runUntil(int& cycles)
{
    int remaining = overflow - counter;

    if (cycles <= remaining)
    {
        run(cycles);
        cycles = 0;
    }
    else
    {
        run(remaining);
        cycles -= remaining;
    }
}

u8 TimerController::readByte(u32 addr)
{
    switch (addr)
    {
    CASE2(REG_TM0CNT_L)
        runTimers();
        return timers[0].data.read(addr - REG_TM0CNT_L);

    CASE2(REG_TM1CNT_L)
        runTimers();
        return timers[1].data.read(addr - REG_TM1CNT_L);

    CASE2(REG_TM2CNT_L)
        runTimers();
        return timers[2].data.read(addr - REG_TM2CNT_L);

    CASE2(REG_TM3CNT_L)
        runTimers();
        return timers[3].data.read(addr - REG_TM3CNT_L);

    // Todo: read 1
    READ2(REG_TM0CNT_H, timers[0].control);
    READ2(REG_TM1CNT_H, timers[1].control);
    READ2(REG_TM2CNT_H, timers[2].control);
    READ2(REG_TM3CNT_H, timers[3].control);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void TimerController::writeByte(u32 addr, u8 byte)
{
    runTimers();

    int enabled;

    switch (addr)
    {
    WRITE2(REG_TM0CNT_L, timers[0].data);
    WRITE2(REG_TM1CNT_L, timers[1].data);
    WRITE2(REG_TM2CNT_L, timers[2].data);
    WRITE2(REG_TM3CNT_L, timers[3].data);

    case REG_TM0CNT_H:
        enabled = timers[0].control.enabled;
        timers[0].control.write(0, byte);
        if (!enabled && timers[0].control.enabled)
            timers[0].init();
        break;

    case REG_TM1CNT_H:
        enabled = timers[1].control.enabled;
        timers[1].control.write(0, byte);
        if (!enabled && timers[1].control.enabled)
            timers[1].init();
        break;

    case REG_TM2CNT_H:
        enabled = timers[2].control.enabled;
        timers[2].control.write(0, byte);
        if (!enabled && timers[2].control.enabled)
            timers[2].init();
        break;

    case REG_TM3CNT_H:
        enabled = timers[3].control.enabled;
        timers[3].control.write(0, byte);
        if (!enabled && timers[3].control.enabled)
            timers[3].init();
        break;
    }
    schedule();
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

    overflow = 0x3FFF'FFFF;

    for (auto& timer : timers)
    {
        if (timer.control.enabled && !timer.control.cascade)
        {
            active.push_back(&timer);

            overflow = std::min(overflow, timer.nextOverflow());
        }
    }
}
