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

    timers[1].prev = &timers[0];
    timers[2].prev = &timers[1];
    timers[3].prev = &timers[2];
}

void TimerController::reset()
{
    active.clear();

    for (auto& timer : timers)
    {
        timer.reset();
    }

    threshold   = 0xFFFFFFFF;
    counter = 0;
}

void TimerController::run(u64 cycles)
{
    counter += cycles;

    if (counter >= threshold)
    {
        update();

        counter = 0;
        last    = 0;
    }
}

void TimerController::runUntil(int& cycles)
{
    u64 rem = threshold - counter;

    if (cycles <= rem)
    {
        run(cycles);
        cycles = 0;
    }
    else
    {
        run(cycles - rem);
        cycles -= rem;
    }
}

u8 TimerController::readByte(u32 addr)
{
    switch (addr)
    {
    CASE2(REG_TM0CNT_L)
        if (timers[0].canChange())
        {
            update();
            timers[0].update();
        }
        return timers[0].data.read(addr - REG_TM0CNT_L);

    CASE2(REG_TM1CNT_L)
        if (timers[1].canChange())
        {
            update();
            timers[1].update();
        }
        return timers[1].data.read(addr - REG_TM1CNT_L);

    CASE2(REG_TM2CNT_L)
        if (timers[2].canChange())
        {
            update();
            timers[2].update();
        }
        return timers[2].data.read(addr - REG_TM2CNT_L);

    CASE2(REG_TM3CNT_L)
        if (timers[3].canChange())
        {
            update();
            timers[3].update();
        }
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
    update();

    switch (addr)
    {
    WRITE2(REG_TM0CNT_L, timers[0].data);
    WRITE2(REG_TM1CNT_L, timers[1].data);
    WRITE2(REG_TM2CNT_L, timers[2].data);
    WRITE2(REG_TM3CNT_L, timers[3].data);

    case REG_TM0CNT_H:
    {
        int enabled = timers[0].control.enabled;
        timers[0].control.write(0, byte);
        if (!enabled && timers[0].control.enabled)
            timers[0].start();
        break;
    }

    case REG_TM1CNT_H:
    {
        int enabled = timers[1].control.enabled;
        timers[1].control.write(0, byte);
        if (!enabled && timers[1].control.enabled)
            timers[1].start();
        break;
    }

    case REG_TM2CNT_H:
    {
        int enabled = timers[2].control.enabled;
        timers[2].control.write(0, byte);
        if (!enabled && timers[2].control.enabled)
            timers[2].start();
        break;
    }

    case REG_TM3CNT_H:
    {
        int enabled = timers[3].control.enabled;
        timers[3].control.write(0, byte);
        if (!enabled && timers[3].control.enabled)
            timers[3].start();
        break;
    }
    }
    rebuild();
}

void TimerController::update()
{
    for (auto& timer : active)
    {
        timer->run(counter - last);
    }
    last = counter;
}

void TimerController::rebuild()
{
    last      = 0;
    counter   = 0;
    threshold = 0xFFFFFFFFull;

    active.clear();

    for (auto& timer : timers)
    {
        if (timer.control.enabled && !timer.control.cascade)
        {
            active.push_back(&timer);
        }
        if (timer.canCauseInterrupt())
        {
            threshold = std::min(threshold, timer.interruptsAfter());
        }
    }
}
