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
}

void ARM::run(int cycles)
{
    remaining += cycles;

    while (remaining > 0)
    {
        int last = remaining;

        if (dmac.active)
        {
            dmac.run(remaining);
        }
        else
        {
            if (io.haltcnt)
            {
                timerc.runUntil(remaining);
                continue;
            }
            execute();
        }
        timerc.run(last - remaining);
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

void ARM::execute()
{
    //disasm();

    if (!cpsr.i && irqh.requested)
    {
        interruptHW();
    }
    else
    {
        if (cpsr.t)
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

void ARM::disasm()
{
    static u32 operation = 0;
    operation++;

    DisasmData data;

    data.thumb = cpsr.t;
    data.instr = pipe[0];
    data.pc    = pc;
    data.lr    = lr;

    fmt::printf("%08X  %08X  %08X  %s\n", 
        operation,
        pc - 2 * cpsr.size(),
        data.instr, 
        disassemble(data)
    );
}

void ARM::idle()
{
    remaining--;
}

void ARM::booth(u32 multiplier, bool ones)
{
    static constexpr u32 masks[3] =
    {
        0xFF00'0000,
        0xFFFF'0000,
        0xFFFF'FF00
    };

    int internal = 4;
    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (ones && bits == mask))
            internal--;
        else
            break;
    }
    remaining -= internal;
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
