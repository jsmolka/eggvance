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

template<uint State>
void Arm::dispatch()
{
    while (cycles > 0 && state == State)
    {
        if (State & kStateDma)
        {
            dma.run();
        }
        else
        {
            if (State & kStateHalt)
            {
                if (State & kStateTimer)
                    timer.runUntilIrq(cycles);
                else
                    cycles = 0;
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
}

void Arm::tick(int cycles)
{
    this->cycles -= cycles;

    if (state & kStateTimer)
        timer.run(cycles);

    while (irq.delay && cycles)
    {
        --irq.delay;
        --cycles;
    }
}

void Arm::idle(int cycles)
{
    pipe.access = Access::NonSequential;

    tickRam(cycles);
}

void Arm::tickMul(u32 multiplier, bool sign)
{
    int cycles = 1;

    if (sign)
    {
        cycles += (multiplier >>  8) != 0 && (multiplier >>  8) != 0xFF'FFFF;
        cycles += (multiplier >> 16) != 0 && (multiplier >> 16) != 0x00'FFFF;
        cycles += (multiplier >> 24) != 0 && (multiplier >> 24) != 0x00'00FF;
    }
    else
    {
        cycles += (multiplier >>  8) != 0;
        cycles += (multiplier >> 16) != 0;
        cycles += (multiplier >> 24) != 0;
    }
    idle(cycles);
}
