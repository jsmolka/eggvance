#include "arm.h"

#include <fmt/printf.h>
#include <common/macros.h>
#include <mmu/mmu.h>

#include "disassemble.h"
#include "decode.h"

ARM arm;

ARM::ARM()
{
    Arm_GenerateLut();
    Thumb_GenerateLut();
}

void ARM::reset()
{
    Registers::reset();

    io.int_master.reset();
    io.int_enabled.reset();
    io.int_request.reset();
    io.waitcnt.reset();
    io.haltcnt.reset();

    dma.reset();
    timer.reset();

    remaining = 0;

    flushPipeWord();
    instr_size = 4;
    pc += instr_size;
}

void ARM::run(int cycles)
{
    remaining += cycles;

    while (remaining > 0)
    {
        int last = remaining;

        if (dma.active)
        {
            dma.run(remaining);
        }
        else
        {
            if (io.haltcnt)
            {
                timer.runUntil(remaining);
                continue;
            }
            execute();
        }
        timer.run(last - remaining);
    }
}

void ARM::request(Interrupt flag)
{
    int mask = static_cast<int>(flag);

    io.int_request |= mask;

    if (io.int_enabled & mask)
        io.haltcnt = false;
}

void ARM::execute()
{
    //disasm();

    if (interrupted())
    {
        interruptHW();
    }
    else
    {
        if (cpsr.thumb)
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

            if (cpsr.check(PSR::Condition(instr >> 28)))
            {
                (this->*instr_arm[armHash(instr)])(instr);
            }
        }
    }
    pc += instr_size;
}

void ARM::disasm()
{
    static u32 operation = 0;
    operation++;

    DisasmData data;

    data.thumb = cpsr.thumb;
    data.instr = pipe[0];
    data.pc    = pc;
    data.lr    = lr;

    fmt::printf("%08X  %08X  %08X  %s\n", 
        operation,
        pc - 2 * instr_size,
        data.instr, 
        disassemble(data)
    );
}

void ARM::flushPipeHalf()
{
    pipe[0] = readHalf(pc + 0);
    pipe[1] = readHalf(pc + 2);
    pc += 2;
}

void ARM::flushPipeWord()
{
    pipe[0] = readWord(pc + 0);
    pipe[1] = readWord(pc + 4);
    pc += 4;
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
    u32 cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->lr = lr;
    this->pc = pc;

    this->cpsr.thumb = false;
    this->cpsr.irqd  = true;

    flushPipeWord();
    instr_size = 4;
}

void ARM::interruptHW()
{
    u32 lr = pc - 2 * instr_size + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::interruptSW()
{
    u32 lr = pc - instr_size;

    interrupt(0x08, lr, PSR::Mode::SVC);
}

bool ARM::interrupted() const
{
    return !cpsr.irqd
        && io.int_master
        && io.int_enabled & io.int_request;
}
