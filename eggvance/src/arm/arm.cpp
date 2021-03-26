#include "arm.h"

#include <shell/macros.h>

#include "constants.h"
#include "decode.h"
#include "base/config.h"
#include "dma/dma.h"
#include "scheduler/scheduler.h"
#include "timer/timer.h"

Arm::Arm()
{
    events.interrupt = [this](u64 late)
    {
        state |= kStateIrq;
    };
}

void Arm::init()
{
    flushWord();
    pc += 4;
}

void Arm::run(u64 cycles)
{
    target += cycles;

    while (scheduler.now < target)
    {
        switch (state)
        {
        SHELL_CASE16(0, dispatch<kLabel>())

        default:
            SHELL_UNREACHABLE;
            break;
        }
    }
}

template<uint State>
void Arm::dispatch()
{
    while (scheduler.now < target && state == State)
    {
        if (State & kStateDma)
        {
            dma.run();
        }
        else if (State & kStateHalt)
        {
            scheduler.run(std::min(target - scheduler.now, scheduler.next - scheduler.now));
        }
        else
        {
            if (State & kStateIrq && !cpsr.i)
            {
                interruptHw();
            }
            else
            {
                if (State & kStateThumb)
                {
                    u16 instr = pipe[0];

                    pipe[0] = pipe[1];
                    pipe[1] = readHalf(pc, pipe.access);
                    pipe.access = Access::Sequential;

                    std::invoke(instr_thumb[hashThumb(instr)], this, instr);
                }
                else
                {
                    u32 instr = pipe[0];

                    pipe[0] = pipe[1];
                    pipe[1] = readWord(pc, pipe.access);
                    pipe.access = Access::Sequential;

                    if (cpsr.check(instr >> 28))
                    {
                        std::invoke(instr_arm[hashArm(instr)], this, instr);
                    }
                }
            }
            pc += cpsr.size();
        }
    }
}

void Arm::flushHalf()
{
    pc &= ~0x1;
    pipe[0] = readHalf(pc, Access::NonSequential);
    pc += 2;
    pipe[1] = readHalf(pc, Access::Sequential);
    pipe.access = Access::Sequential;
}

void Arm::flushWord()
{
    pc &= ~0x3;
    pipe[0] = readWord(pc, Access::NonSequential);
    pc += 4;
    pipe[1] = readWord(pc, Access::Sequential);
    pipe.access = Access::Sequential;
}
