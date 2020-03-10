#include "timerc.h"

#include <algorithm>

#include "arm/arm.h"
#include "common/macros.h"
#include "mmu/memmap.h"

TimerController timerc;

TimerController::TimerController()
{
    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    reset();
}

void TimerController::reset()
{
    active.clear();

    timers[0] = Timer(0);
    timers[1] = Timer(1);
    timers[2] = Timer(2);
    timers[3] = Timer(3);

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

u8 TimerController::read(u32 addr)
{
    #define READ_DATA_REG(label, reg)                       \
        case label + 0: runTimers(); return reg.read<0>();  \
        case label + 1: runTimers(); return reg.read<1>()

    switch (addr)
    {
    READ_DATA_REG(REG_TM0CNT_L, timers[0].io.data   );
    READ_HALF_REG(REG_TM0CNT_H, timers[0].io.control);
    READ_DATA_REG(REG_TM1CNT_L, timers[1].io.data   );
    READ_HALF_REG(REG_TM1CNT_H, timers[1].io.control);
    READ_DATA_REG(REG_TM2CNT_L, timers[2].io.data   );
    READ_HALF_REG(REG_TM2CNT_H, timers[2].io.control);
    READ_DATA_REG(REG_TM3CNT_L, timers[3].io.data   );
    READ_HALF_REG(REG_TM3CNT_H, timers[3].io.control);

    default:
        UNREACHABLE;
        return 0;
    }

    #undef READ_DATA_REG
}

void TimerController::write(u32 addr, u8 byte)
{
    #define WRITE_CTRL_REG(label, timer)                \
        case label:                                     \
        {                                               \
            runTimers();                                \
            int enabled = timer.io.control.enabled;     \
            timer.io.control.write<0>(byte & 0xC7);     \
            if (!enabled && timer.io.control.enabled)   \
                timer.start();                          \
            else if (enabled)                           \
                timer.update();                         \
            schedule();                                 \
            break;                                      \
        }

    switch (addr)
    {
    WRITE_HALF_REG(REG_TM0CNT_L, timers[0].io.data, 0x0000'FFFF);
    WRITE_HALF_REG(REG_TM1CNT_L, timers[1].io.data, 0x0000'FFFF);
    WRITE_HALF_REG(REG_TM2CNT_L, timers[2].io.data, 0x0000'FFFF);
    WRITE_HALF_REG(REG_TM3CNT_L, timers[3].io.data, 0x0000'FFFF);
    WRITE_CTRL_REG(REG_TM0CNT_H, timers[0]                     );
    WRITE_CTRL_REG(REG_TM1CNT_H, timers[1]                     );
    WRITE_CTRL_REG(REG_TM2CNT_H, timers[2]                     );
    WRITE_CTRL_REG(REG_TM3CNT_H, timers[3]                     );

    default:
        UNREACHABLE;
        break;
    }

    #undef WRITE_CTRL_REG
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
        if (timer.io.control.enabled && !timer.io.control.cascade)
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
