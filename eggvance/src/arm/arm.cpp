#include "arm.h"

#include "arm/decode.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "timer/timer.h"

Arm::Arm()
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
                    interruptHW();
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

void Arm::interrupt(u32 pc, u32 lr, PSR::Mode mode)
{
    PSR cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->cpsr.t = 0;
    this->cpsr.i = 1;

    this->lr = lr;
    this->pc = pc;

    flushWord();
    state &= ~kStateThumb;
}

void Arm::interruptHW()
{
    u32 lr = pc - 2 * cpsr.size() + 4;

    interrupt(0x18, lr, PSR::Mode::kModeIrq);
}

void Arm::interruptSW()
{
    u32 lr = pc - cpsr.size();

    interrupt(0x08, lr, PSR::Mode::kModeSvc);
}
