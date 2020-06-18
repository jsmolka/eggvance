#include "timerc.h"

#include <algorithm>

#include "arm/arm.h"
#include "base/constants.h"
#include "base/macros.h"

constexpr auto kOverflowInitial = 0x4000'0000;

TimerController timerc;

TimerController::TimerController()
{
    reset();
}

void TimerController::reset()
{
    active_timers.clear();

    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    counter = 0;
    overflow = kOverflowInitial;
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
    READ_DATA_REG(REG_TM0CNT_L, timers[0].io.data);
    READ_HALF_REG(REG_TM0CNT_H, timers[0].io.ctrl);
    READ_DATA_REG(REG_TM1CNT_L, timers[1].io.data);
    READ_HALF_REG(REG_TM1CNT_H, timers[1].io.ctrl);
    READ_DATA_REG(REG_TM2CNT_L, timers[2].io.data);
    READ_HALF_REG(REG_TM2CNT_H, timers[2].io.ctrl);
    READ_DATA_REG(REG_TM3CNT_L, timers[3].io.data);
    READ_HALF_REG(REG_TM3CNT_H, timers[3].io.ctrl);

    default:
        UNREACHABLE;
        return 0;
    }

    #undef READ_DATA_REG
}

void TimerController::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE_HALF_REG(REG_TM0CNT_L, timers[0].io.data, 0xFFFF);
    WRITE_HALF_REG(REG_TM1CNT_L, timers[1].io.data, 0xFFFF);
    WRITE_HALF_REG(REG_TM2CNT_L, timers[2].io.data, 0xFFFF);
    WRITE_HALF_REG(REG_TM3CNT_L, timers[3].io.data, 0xFFFF);

    case REG_TM0CNT_H: writeControl(timers[0], byte); break;
    case REG_TM1CNT_H: writeControl(timers[1], byte); break;
    case REG_TM2CNT_H: writeControl(timers[2], byte); break;
    case REG_TM3CNT_H: writeControl(timers[3], byte); break;

    default:
        UNREACHABLE;
        break;
    }
}

void TimerController::writeControl(Timer& timer, u8 byte)
{
    runTimers();
    uint enable = timer.io.ctrl.enable;   
    timer.io.ctrl.write<0>(byte & 0xC7);  
    if (!enable && timer.io.ctrl.enable)  
        timer.init();                    
    else if (enable)                      
        timer.update();                   
    schedule();                           
}

void TimerController::runTimers()
{
    for (Timer& timer : active_timers)
    {
        timer.run(counter);
    }
    overflow -= counter;
    counter = 0;
}

void TimerController::schedule()
{
    active_timers.clear();

    overflow = kOverflowInitial;

    for (auto& timer : timers)
    {
        if (timer.io.ctrl.enable && !timer.io.ctrl.cascade)
        {
            active_timers.push_back(std::ref(timer));

            overflow = std::min(overflow, timer.nextOverflow());
        }
    }

    if (active_timers.size() > 0)
        arm.state |= ARM::kStateTimer;
    else
        arm.state &= ~ARM::kStateTimer;
}

void TimerController::reschedule()
{
    overflow = kOverflowInitial;

    for (Timer& timer : active_timers)
    {
        overflow = std::min(overflow, timer.nextOverflow());
    }
}
