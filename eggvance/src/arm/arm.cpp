#include "arm.h"

#include "constants.h"
#include "decode.h"
#include "base/config.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "timer/timer.h"

Arm::Arm()
{
    irq.master.on_write  = std::bind(&Arm::interruptProcess, this);
    irq.enable.on_write  = std::bind(&Arm::interruptProcess, this);
    irq.request.on_write = std::bind(&Arm::interruptProcess, this);

    postflg.value = config.bios_skip;
}

void Arm::init()
{
    flushWord();
    pc += 4;
}

void Arm::run(int cycles)
{
    this->cycles += cycles;

    while (this->cycles > 0)
    {
        switch (state)
        {
        INDEXED_CASE8( 0, dispatch<kLabel>());
        INDEXED_CASE8( 8, dispatch<kLabel>());
        INDEXED_CASE8(16, dispatch<kLabel>());
        INDEXED_CASE8(24, dispatch<kLabel>());

        default:
            SHELL_UNREACHABLE;
            break;
        }
    }
}

template<uint State>
void Arm::dispatch()
{
    while (cycles > 0 && state == State)
    {
        if (State & kStateDma)
        {
            dma.run();
        }
        else if (State & kStateHalt)
        {
            int event = cycles;

            if (State & kStateIrq && irq.delaying)
            {
                event = std::min(event, irq.delay);
            }

            if (State & kStateTimer)
            {
                event = std::min(event, timer.cycles());
            }

            tick(event);

            if (State & kStateIrq && !cpsr.i && irq.delay == 0)
            {
                irq.delaying = false;

                interruptHw();

                pc += cpsr.size();
            }
        }
        else
        {
            if (State & kStateIrq && !cpsr.i && irq.delay == 0)
            {
                irq.delaying = false;

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
