#include "arm.h"

#include <fmt/printf.h>

#include "common/integer.h"
#include "common/macros.h"
#include "common/utility.h"
//#include "mmu/interrupt.h"
#include "diasm.h"

ARM arm;

void ARM::reset()
{
    Registers::reset();

    cycles = 0;
}

void ARM::run(int cycles)
{
    while (cycles > 0)
    {
        cycles -= execute();
    }
}

void ARM::irq(Interrupt flag)
{
    // Set and check irq registers

    if (!cpsr.irqd)
    {
        interruptHW();
    }
}

int ARM::execute()
{
    u64 last = cycles;

    if (cpsr.thumb)
    {
        u16 instr = mmu.readHalf(pc - 4);

        (this->*instr_thumb[instr >> 6])(instr);
    }
    else
    {
        u32 instr = mmu.readWord(pc - 8);

        if (cpsr.check(PSR::Condition(instr >> 28)))
        {
            int hash = ((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF);

            (this->*instr_arm[hash])(instr);
        }
        else
        {
            cycle<Access::Seq>(pc + 8);
        }
    }
    advance();

    #ifdef EGG_DEBUG
    disasm();
    #endif

    return static_cast<int>(cycles - last);
}

void ARM::interrupt(u32 pc, u32 lr, PSR::Mode mode)
{
    cycle<Access::Nonseq>(this->pc);

    u32 cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->lr = lr;
    this->pc = pc;

    this->cpsr.thumb = false;
    this->cpsr.irqd  = true;

    refill<State::Arm>();
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
