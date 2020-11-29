#include "arm.h"

#include <utility>

#include "constants.h"
#include "decode.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "timer/timer.h"

Arm::Arm()
{
    irq.master.process  = std::bind(&Arm::processIrq, this);
    irq.enable.process  = std::bind(&Arm::processIrq, this);
    irq.request.process = std::bind(&Arm::processIrq, this);
}

void Arm::init()
{
    pipe[0] = readWord(pc + 0);
    pipe[1] = readWord(pc + 4);
    pc += 8;
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

void Arm::flushHalf()
{
    pc &= ~0x1;
    pipe[0] = readHalf(pc + 0);
    pipe[1] = readHalf(pc + 2);
    pc += 2;
}

void Arm::flushWord()
{
    pc &= ~0x3;
    pipe[0] = readWord(pc + 0);
    pipe[1] = readWord(pc + 4);
    pc += 4;
}

template<uint State>
void Arm::dispatch()
{
    while (cycles > 0 && state == State)
    {
        int previous = cycles;

        if (State & kStateDma)
        {
            dma.run(cycles);
        }
        else
        {
            if (State & kStateHalt)
            {
                if (State & kStateTimer)
                    timer.runUntilIrq(cycles);
                else
                    cycles = 0;

                return;
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
                        pipe[1] = readHalf(pc);

                        std::invoke(instr_thumb[hashThumb(instr)], this, instr);
                    }
                    else
                    {
                        u32 instr = pipe[0];

                        pipe[0] = pipe[1];
                        pipe[1] = readWord(pc);

                        if (cpsr.check(instr >> 28))
                        {
                            std::invoke(instr_arm[hashArm(instr)], this, instr);
                        }
                    }
                }
                pc += cpsr.size();
            }
        }

        if (state & kStateTimer)
            timer.run(previous - cycles);
    }
}

void Arm::idle()
{
    cycles--;
}
