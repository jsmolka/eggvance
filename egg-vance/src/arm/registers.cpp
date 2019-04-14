#include "registers.h"

#include <cstring>

#include "common/log.h"

void Registers::reset()
{
    std::memset(regs, 0, sizeof(regs));
    std::memset(bank, 0, sizeof(bank));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    pc = 0x8000000;
    
    sp = 0x03007F00;
    bank[BANK_FIQ].sp = 0x03007F00;
    bank[BANK_ABT].sp = 0x03007F00;
    bank[BANK_UND].sp = 0x03007F00;
    bank[BANK_SVC].sp = 0x03007FE0;
    bank[BANK_IRQ].sp = 0x03007FA0;

    cpsr = 0x5F;
}

bool Registers::isThumb() const
{
    return cpsr & CPSR_T;
}

void Registers::setThumb(bool set)
{
    setFlag(CPSR_T, set);
}

bool Registers::isArm() const
{
    return (cpsr & CPSR_T) == 0;
}

int Registers::z() const
{
    return (cpsr & CPSR_Z) ? 1 : 0;
}

int Registers::n() const
{
    return (cpsr & CPSR_N) ? 1 : 0;
}

int Registers::c() const
{
    return (cpsr & CPSR_C) ? 1 : 0;
}

int Registers::v() const
{
    return (cpsr & CPSR_V) ? 1 : 0;
}

void Registers::setZ(bool set)
{
    setFlag(CPSR_Z, set);
}

void Registers::setN(bool set)
{
    setFlag(CPSR_N, set);
}

void Registers::setC(bool set)
{
    setFlag(CPSR_C, set);
}

void Registers::setV(bool set)
{
    setFlag(CPSR_V, set);
}

bool Registers::checkCondition(Condition cond) const
{
    if (cond == COND_AL)
        return true;

    switch (cond)
    {
    case COND_EQ: return z();
    case COND_NE: return !z();
    case COND_CS: return c();
    case COND_CC: return !c();
    case COND_MI: return n();
    case COND_PL: return !n();
    case COND_VS: return v();
    case COND_VC: return !v();
    case COND_HI: return c() && !z();
    case COND_LS: return !c() || z();
    case COND_GE: return n() == v();
    case COND_LT: return n() != v();
    case COND_GT: return !z() && (n() == v());
    case COND_LE: return z() || (n() != v());
    case COND_AL: return true;
    case COND_NV: return false;

    default:
        log() << "Invalid condition " << (int)cond;
        return true;
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

void Registers::setFlag(CPSR flag, bool set)
{
    if (set)
        cpsr |= flag;
    else
        cpsr &= ~flag;
}
