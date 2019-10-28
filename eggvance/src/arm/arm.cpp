#include "arm.h"

#include <fmt/printf.h>

#include "common/macros.h"
#include "mmu/mmu.h"
#include "decode.h"
#include "diasm.h"

ARM arm;

void ARM::reset()
{
    Registers::reset();

    io.irq_master.reset();
    io.irq_enabled.reset();
    io.irq_request.reset();
    io.waitcnt.reset();
    io.haltcnt.reset();

    dma.reset();
    timer.reset();

    cycles = 0;

    flushPipeWord();
    pc += 4;
}

void ARM::run(int cycles_)
{
    cycles += cycles_;

    while (cycles > 0)
    {
        int last = cycles;

        if (dma.active)
        {
            dma.run(cycles);
            timer.run(last - cycles);
        }
        else
        {
            if (io.haltcnt)
            {
                timer.runUntil(cycles);
            }
            else
            {
                execute();
                timer.run(last - cycles);
            }
        }
    }
}

void ARM::request(Interrupt flag)
{
    int mask = static_cast<int>(flag);

    if (io.irq_enabled & mask)
        io.haltcnt = false;

    io.irq_request |= mask;
}

void ARM::execute()
{
    //#ifdef EGG_DEBUG
    //disasm();
    //#endif

    if (io.irq_master && (io.irq_enabled & io.irq_request) && !cpsr.irqd)
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

            (this->*instr_thumb[instr >> 6])(instr);
        }
        else
        {
            u32 instr = pipe[0];

            pipe[0] = pipe[1];
            pipe[1] = readWord(pc);

            if (cpsr.check(PSR::Condition(instr >> 28)))
            {
                (this->*instr_arm[((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF)])(instr);
            }
        }
    }
    pc += cpsr.thumb ? 2 : 4;
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
    cycles--;
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
    cycles -= internal;
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
}

void ARM::interruptHW()
{
    // Returns with subs pc, lr, 4
    u32 lr = pc - 2 * (cpsr.thumb ? 2 : 4) + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::interruptSW()
{
    // Returns with movs pc, lr
    u32 lr = pc - (cpsr.thumb ? 2 : 4);

    interrupt(0x08, lr, PSR::Mode::SVC);
}

void ARM::disasm()
{
    u32 instr = cpsr.thumb
        ? mmu.readHalf(pc - 4)
        : mmu.readWord(pc - 8);

    fmt::printf("%08X  %08X  %08X  %s\n", 
        cycles, 
        pc - 2 * (cpsr.thumb ? 2 : 4), 
        instr, 
        Disassembler::disassemble(instr, *this)
    );
}
