#include "arm.h"

#include "arm/decode.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "timer/timer.h"

Arm::Arm()
{
    irq.master.process  = std::bind(&Arm::processIrq, this);
    irq.enable.process  = std::bind(&Arm::processIrq, this);
    irq.request.process = std::bind(&Arm::processIrq, this);

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
            EGGCPT_UNREACHABLE;
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

template<uint state>
void Arm::dispatch()
{
    while (cycles > 0 && this->state == state)
    {
        int previous = cycles;

        if (state & kStateDma)
        {
            dma.run(cycles);
        }
        else
        {
            if (state & kStateHalt)
            {
                if (state & kStateTimer)
                    timer.runUntilIrq(cycles);
                else
                    cycles = 0;

                return;
            }
            else
            {
                if (state & kStateIrq && !cpsr.i)
                {
                    interruptHw();
                }
                else
                {
                    if (state & kStateThumb)
                    {
                        u16 instr = pipe[0];

                        pipe[0] = pipe[1];
                        pipe[1] = readHalf(pc);

                        (this->*instr_thumb[hashThumb(instr)])(instr);
                    }
                    else
                    {
                        u32 instr = pipe[0];

                        pipe[0] = pipe[1];
                        pipe[1] = readWord(pc);

                        if (cpsr.check(instr >> 28))
                        {
                            (this->*instr_arm[hashArm(instr)])(instr);
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

void Arm::booth(u32 multiplier, bool sign)
{
    static constexpr u32 masks[3] = {
        0xFF00'0000,
        0xFFFF'0000,
        0xFFFF'FF00
    };

    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (sign && bits == mask))
            cycles++;
        else
            break;
    }
    cycles -= 4;
}
