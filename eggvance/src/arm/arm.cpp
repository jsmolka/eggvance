#include "arm.h"

#include <fmt/printf.h>

#include "arm/decode.h"
#include "arm/disassemble.h"
#include "dma/dmac.h"
#include "irq/irqh.h"
#include "mmu/mmu.h"
#include "timer/timerc.h"

ARM arm;

ARM::ARM()
{
    flushWord();
    pc += 4;
}

void ARM::run(int cycles)
{
    this->cycles += cycles;

    #define DISPATCH_CASE(state)                                \
        case state + 0b000: dispatch<state + 0b000>(); break;   \
        case state + 0b001: dispatch<state + 0b001>(); break;   \
        case state + 0b010: dispatch<state + 0b010>(); break;   \
        case state + 0b011: dispatch<state + 0b011>(); break;   \
        case state + 0b100: dispatch<state + 0b100>(); break;   \
        case state + 0b101: dispatch<state + 0b101>(); break;   \
        case state + 0b110: dispatch<state + 0b110>(); break;   \
        case state + 0b111: dispatch<state + 0b111>(); break

    while (this->cycles > 0)
    {
        switch (state)
        {
        DISPATCH_CASE(0b00000);
        DISPATCH_CASE(0b01000);
        DISPATCH_CASE(0b10000);
        DISPATCH_CASE(0b11000);

        default:
            UNREACHABLE;
            break;
        }
    }

    #undef DISPATCH_CASE
}

void ARM::flushHalf()
{
    pc &= ~0x1;
    pipe[0] = readHalf(pc + 0);
    pipe[1] = readHalf(pc + 2);
    pc += 2;
}

void ARM::flushWord()
{
    pc &= ~0x3;
    pipe[0] = readWord(pc + 0);
    pipe[1] = readWord(pc + 4);
    pc += 4;
}

template<uint state>
void ARM::dispatch()
{
    while (cycles > 0 && this->state == state)
    {
        int previous = cycles;

        if (state & kStateDma)
        {
            dmac.run(cycles);
        }
        else
        {
            if (state & kStateHalt)
            {
                if (state & kStateTimer)
                    timerc.runUntilIrq(cycles);
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
                    //disasm();

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
            timerc.run(previous - cycles);
    }
}

void ARM::disasm() const
{
    static u32 operation = 0;
    operation++;

    fmt::printf("%08X  %08X  %08X  %s\n", 
        operation,
        pc - 2 * cpsr.size(),
        pipe[0], 
        disassemble(pipe[0], pc, lr, cpsr.t)
    );
}

void ARM::idle()
{
    cycles--;
}

void ARM::booth(u32 multiplier, bool sign)
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

void ARM::interrupt(u32 pc, u32 lr, PSR::Mode mode)
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

void ARM::interruptHW()
{
    u32 lr = pc - 2 * cpsr.size() + 4;

    interrupt(0x18, lr, PSR::Mode::kModeIrq);
}

void ARM::interruptSW()
{
    u32 lr = pc - cpsr.size();

    interrupt(0x08, lr, PSR::Mode::kModeSvc);
}
