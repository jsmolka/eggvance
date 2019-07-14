#include "registers.h"

#include <cstring>

static bool use_bios = false;

Registers::Registers()
    : thumb(cpsr)
    , fiq_disable(cpsr)
    , irq_disable(cpsr)
    , v(cpsr)
    , c(cpsr)
    , z(cpsr)
    , n(cpsr)
{

}

void Registers::reset()
{
    std::memset(regs,     0, sizeof(regs));
    std::memset(bank,     0, sizeof(bank));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    if (!use_bios)
    {
        pc = 0x8000000;
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
        cpsr = 0xD3;
    }
}

bool Registers::check(Condition condition) const
{
    if (condition == COND_AL)
        return true;

    switch (condition)
    {
    case COND_EQ: return z;
    case COND_NE: return !z;
    case COND_CS: return c;
    case COND_CC: return !c;
    case COND_MI: return n;
    case COND_PL: return !n;
    case COND_VS: return v;
    case COND_VC: return !v;
    case COND_HI: return c && !z;
    case COND_LS: return !c || z;
    case COND_GE: return n == v;
    case COND_LT: return n != v;
    case COND_GT: return !z && (n == v);
    case COND_LE: return z || (n != v);
    case COND_AL: return true;
    case COND_NV: return false;
    }
    return true;
}

u32 Registers::operator[](int index) const
{
    return regs[index];
}

u32& Registers::operator[](int index)
{
    return regs[index];
}
