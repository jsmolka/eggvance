#include "arm.h"

#include "decode.h"
#include "dma/dma.h"
#include "scheduler/scheduler.h"
#include "timer/timer.h"

Arm::Arm()
{
    irq.delay = [this](u64 late)
    {
        state |= State::Irq;
    };

    pipe[0] = 0xF000'0000;
    pipe[1] = 0xF000'0000;
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
        if (State & State::Dma)
        {
            dma.run();
        }
        else if (State & State::Halt)
        {
            scheduler.run(std::min(target - scheduler.now, scheduler.next - scheduler.now));
        }
        else
        {
            if (State & State::Irq && !cpsr.i)
            {
                interruptHw();
            }
            else
            {
                if (State & State::Thumb)
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
