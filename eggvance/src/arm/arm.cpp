#include "arm.h"

#include <fmt/printf.h>

#include "decode.h"
#include "disassemble.h"
#include "common/mpl.h"
#include "mmu/mmu.h"
#include "system/dmacontroller.h"
#include "system/irqhandler.h"
#include "system/timercontroller.h"

ARM arm;

void ARM::reset()
{
    Registers::reset();

    io.waitcnt.reset();
    io.haltcnt.reset();

    updateDispatch();

    cycles = 0;
    flushWord();
    pc += 4;
}

void ARM::run(uint cycles)
{
    this->cycles += cycles;

    while (this->cycles > 0)
        (this->*dispatch)();
}

void ARM::updateDispatch()
{
    static constexpr auto dispatchers = mpl::array<Dispatch, 32>([](auto x) {
        return &ARM::execute<static_cast<uint>(x)>;
    });

    uint flags = 0;

    flags |= cpsr.t << 0;
    flags |= io.haltcnt.halt << 1;
    flags |= irqh.requested << 2;
    flags |= (dmac.active ? 1 : 0) << 3;
    flags |= (timerc.active.size() > 0 ? 1 : 0) << 4;

    dispatch = dispatchers[flags];
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

    int last = cycles;

    if (dma)
    {
        dmac.run(cycles);
    }
    else
    {
        if (halt)
        {
            timerc.runUntil(cycles);
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
        timerc.run(last - cycles);
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
