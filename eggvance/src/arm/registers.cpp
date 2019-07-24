#include "registers.h"

#include <cstring>

static bool use_bios = false;

Registers::Registers()
    : thumb(cpsr)
    , fiqd(cpsr)
    , irqd(cpsr)
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
        pc = 0x8;
        cpsr = 0xD3;
    }
}

bool Registers::check(Condition condition) const
{
    if (condition == Condition::AL)
        return true;

    switch (condition)
    {
    case Condition::EQ: return z;
    case Condition::NE: return !z;
    case Condition::CS: return c;
    case Condition::CC: return !c;
    case Condition::MI: return n;
    case Condition::PL: return !n;
    case Condition::VS: return v;
    case Condition::VC: return !v;
    case Condition::HI: return c && !z;
    case Condition::LS: return !c || z;
    case Condition::GE: return n == v;
    case Condition::LT: return n != v;
    case Condition::GT: return !z && (n == v);
    case Condition::LE: return z || (n != v);
    case Condition::AL: return true;
    case Condition::NV: return false;
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
