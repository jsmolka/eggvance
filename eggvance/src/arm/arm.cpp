#include "arm.h"

#include <fmt/printf.h>

#include "common/integer.h"
#include "common/macros.h"
#include "common/utility.h"
#include "diasm.h"

ARM arm;

ARM::ARM()
{
    timers[0].id = 0;
    timers[1].id = 1;
    timers[2].id = 2;
    timers[3].id = 3;

    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];
}

void ARM::reset()
{
    Registers::reset();

    io.int_master.reset();
    io.int_enabled.reset();
    io.int_request.reset();
    io.halt = false;

    for (auto& timer : timers)
        timer.reset();

    dma.reset();

    cycles = 0;
}

void ARM::run(int cycles)
{
    while (cycles > 0)
    {
        if (dma.active)
        {
            // Todo: timers
            dma.run(cycles);
        }
        else
        {
            if (io.halt)
            {
                // Todo: run till first interrupt
                for (auto& timer : timers)
                    timer.run(cycles);
                return;
            }
            int c = execute();
            for (auto& timer : timers)
                timer.run(c);
            cycles -= c;
        }
    }
}

void ARM::request(Interrupt flag)
{
    if (io.int_enabled & static_cast<int>(flag))
        io.halt = false;

    io.int_request |= static_cast<int>(flag);
}

int ARM::execute()
{
    u64 last = cycles;

    //#ifdef EGG_DEBUG
    //disasm();
    //#endif

    if (io.int_master && (io.int_enabled & io.int_request) && !cpsr.irqd)
    {
        interruptHW();
    }
    else
    {
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
    }
    advance();

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
