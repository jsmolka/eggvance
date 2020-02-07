#include "arm.h"

#include <fmt/printf.h>

#include "decode.h"
#include "disassemble.h"
#include "mmu/mmu.h"
#include "system/dmacontroller.h"
#include "system/irqhandler.h"
#include "system/timercontroller.h"

ARM arm;

void ARM::reset()
{
    remaining = 0;

    Registers::reset();

    flushWord();

    pc += 4;

    io.waitcnt.reset();
    io.haltcnt.reset();

    updateDispatch();
}

void ARM::run(int cycles)
{
    remaining += cycles;

    while (remaining > 0)
        (this->*dispatch)();
}

void ARM::updateDispatch()
{
    uint thumb = cpsr.t << 0;
    uint halt  = io.haltcnt.halt << 1;
    uint irq   = irqh.requested << 2;
    uint dma   = (dmac.active ? 1 : 0) << 3;
    uint timer = (timerc.active.size() > 0 ? 1 : 0) << 4;

    switch (thumb | halt | irq | dma | timer)
    {
    case 0b00000: dispatch = &ARM::execute<0b00000>; break;
    case 0b00001: dispatch = &ARM::execute<0b00001>; break;
    case 0b00010: dispatch = &ARM::execute<0b00010>; break;
    case 0b00011: dispatch = &ARM::execute<0b00011>; break;
    case 0b00100: dispatch = &ARM::execute<0b00100>; break;
    case 0b00101: dispatch = &ARM::execute<0b00101>; break;
    case 0b00110: dispatch = &ARM::execute<0b00110>; break;
    case 0b00111: dispatch = &ARM::execute<0b00111>; break;
    case 0b01000: dispatch = &ARM::execute<0b01000>; break;
    case 0b01001: dispatch = &ARM::execute<0b01001>; break;
    case 0b01010: dispatch = &ARM::execute<0b01010>; break;
    case 0b01011: dispatch = &ARM::execute<0b01011>; break;
    case 0b01100: dispatch = &ARM::execute<0b01100>; break;
    case 0b01101: dispatch = &ARM::execute<0b01101>; break;
    case 0b01110: dispatch = &ARM::execute<0b01110>; break;
    case 0b01111: dispatch = &ARM::execute<0b01111>; break;
    case 0b10000: dispatch = &ARM::execute<0b10000>; break;
    case 0b10001: dispatch = &ARM::execute<0b10001>; break;
    case 0b10010: dispatch = &ARM::execute<0b10010>; break;
    case 0b10011: dispatch = &ARM::execute<0b10011>; break;
    case 0b10100: dispatch = &ARM::execute<0b10100>; break;
    case 0b10101: dispatch = &ARM::execute<0b10101>; break;
    case 0b10110: dispatch = &ARM::execute<0b10110>; break;
    case 0b10111: dispatch = &ARM::execute<0b10111>; break;
    case 0b11000: dispatch = &ARM::execute<0b11000>; break;
    case 0b11001: dispatch = &ARM::execute<0b11001>; break;
    case 0b11010: dispatch = &ARM::execute<0b11010>; break;
    case 0b11011: dispatch = &ARM::execute<0b11011>; break;
    case 0b11100: dispatch = &ARM::execute<0b11100>; break;
    case 0b11101: dispatch = &ARM::execute<0b11101>; break;
    case 0b11110: dispatch = &ARM::execute<0b11110>; break;
    case 0b11111: dispatch = &ARM::execute<0b11111>; break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::flush()
{
    if (cpsr.t)
        flushHalf();
    else
        flushWord();
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

template<uint flags>
void ARM::execute()
{
    constexpr uint thumb = flags & (1 << 0);
    constexpr uint halt  = flags & (1 << 1);
    constexpr uint irq   = flags & (1 << 2);
    constexpr uint dma   = flags & (1 << 3);
    constexpr uint timer = flags & (1 << 4);

    int last = remaining;

    if (dma)
    {
        dmac.run(remaining);
    }
    else
    {
        if (halt)
        {
            timerc.runUntil(remaining);
        }
        else
        {
            if (irq && !cpsr.i)
            {
                interruptHW();
            }
            else
            {
                //disasm();

                if (thumb)
                {
                    u16 instr = pipe[0];

                    pipe[0] = pipe[1];
                    pipe[1] = readHalf(pc);

                    (this->*instr_thumb[thumbHash(instr)])(instr);
                }
                else
                {
                    u32 instr = pipe[0];

                    pipe[0] = pipe[1];
                    pipe[1] = readWord(pc);

                    if (cpsr.check(static_cast<Condition>(instr >> 28)))
                    {
                        (this->*instr_arm[armHash(instr)])(instr);
                    }
                }
            }
            pc += cpsr.size();
        }
    }

    if (timer)
        timerc.run(last - remaining);
}

void ARM::disasm()
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
    remaining--;
}

void ARM::booth(u32 multiplier, bool sign)
{
    static constexpr u32 masks[3] =
    {
        0xFF00'0000,
        0xFFFF'0000,
        0xFFFF'FF00
    };

    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (sign && bits == mask))
            remaining++;
        else
            break;
    }
    remaining -= 4;
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

    updateDispatch();
}

void ARM::interruptHW()
{
    u32 lr = pc - 2 * cpsr.size() + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::interruptSW()
{
    u32 lr = pc - cpsr.size();

    interrupt(0x08, lr, PSR::Mode::SVC);
}
