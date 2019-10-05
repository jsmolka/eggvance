#include "registers.h"

#include <cstring>

#include "config.h"

Registers::Registers()
{
    reset();
}

void Registers::reset()
{
    std::memset(regs,     0, sizeof(regs));
    std::memset(bank,     0, sizeof(bank));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    if (!cfg.skip_bios)
    {
        pc = 0x8000008;
        sp = 0x03007F00;
        bank[BANK_FIQ].sp = 0x03007F00;
        bank[BANK_ABT].sp = 0x03007F00;
        bank[BANK_UND].sp = 0x03007F00;
        bank[BANK_SVC].sp = 0x03007FE0;
        bank[BANK_IRQ].sp = 0x03007FA0;
        cpsr = 0x5F;
    }
    else 
    {
        pc   = 0x08;
        cpsr = 0xD3;
    }
}

u32 Registers::operator[](int index) const
{
    return regs[index];
}

u32& Registers::operator[](int index)
{
    return regs[index];
}
