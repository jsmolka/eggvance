#include "registerbank.h"

#include "common/log.h"

void RegisterBank::reset()
{
    *bank = { };
    *bank_fiq = { };

    sp = 0x03007F00;

    bank[BANK_FIQ].sp = 0x03007F00;
    bank[BANK_ABT].sp = 0x03007F00;
    bank[BANK_UND].sp = 0x03007F00;
    bank[BANK_SVC].sp = 0x03007FE0;
    bank[BANK_IRQ].sp = 0x03007FA0;

    pc = 0x8000000;

    cpsr = 0x5F;

    // For test ROM
    cpsr |= CPSR_T;
    pc = 0x080000C8;
}

void RegisterBank::switchMode(Mode mode)
{
    Bank old_bank = modeToBank(static_cast<Mode>(cpsr & CPSR_M));
    Bank new_bank = modeToBank(mode);

    if (old_bank == BANK_FIQ || new_bank == BANK_FIQ)
    {
        int old_fiq = old_bank == BANK_FIQ;
        int new_fiq = new_bank == BANK_FIQ;

        bank_fiq[old_fiq].r8 = r8;
        bank_fiq[old_fiq].r9 = r9;
        bank_fiq[old_fiq].r10 = r10;
        bank_fiq[old_fiq].r11 = r11;
        bank_fiq[old_fiq].r12 = r12;

        r8  = bank_fiq[new_fiq].r8;
        r9  = bank_fiq[new_fiq].r9;
        r10 = bank_fiq[new_fiq].r10;
        r11 = bank_fiq[new_fiq].r11;
        r12 = bank_fiq[new_fiq].r12;
    }

    bank[old_bank].sp = sp;
    bank[old_bank].lr = lr;

    sp = bank[new_bank].sp;
    lr = bank[new_bank].lr;

    if (new_bank == BANK_DEF)
    {
        // Default modes do not have a SPSR
        spsr = nullptr;
    }
    else
    {
        // Save current CPSR and set pointer
        bank_spsr[new_bank] = cpsr;
        spsr = &bank_spsr[new_bank];
    }

    // Update mode
    cpsr = (cpsr & ~CPSR_M) | mode;
}

RegisterBank::Bank RegisterBank::modeToBank(Mode mode)
{
    switch (mode)
    {
    case MODE_USR:
    case MODE_SYS: return BANK_DEF;
    case MODE_FIQ: return BANK_FIQ;
    case MODE_SVC: return BANK_SVC;
    case MODE_ABT: return BANK_ABT;
    case MODE_IRQ: return BANK_IRQ;
    case MODE_UND: return BANK_UND;

    default:
        log() << "Cannot get bank for mode: " << (int)mode;
        return BANK_DEF;
    }
}

void RegisterBank::setFlag(CPSR flag, bool set)
{
    if (set)
        cpsr |= flag;
    else
        cpsr &= ~flag;
}

void RegisterBank::setZ(bool set)
{
    setFlag(CPSR_Z, set);
}

void RegisterBank::setN(bool set)
{
    setFlag(CPSR_N, set);
}

void RegisterBank::setC(bool set)
{
    setFlag(CPSR_C, set);
}

void RegisterBank::setV(bool set)
{
    setFlag(CPSR_V, set);
}

u8 RegisterBank::z() const
{
    return (cpsr & CPSR_Z) ? 1 : 0;
}

u8 RegisterBank::n() const
{
    return (cpsr & CPSR_N) ? 1 : 0;
}

u8 RegisterBank::c() const
{
    return (cpsr & CPSR_C) ? 1 : 0;
}

u8 RegisterBank::v() const
{
    return (cpsr & CPSR_V) ? 1 : 0;
}

bool RegisterBank::isThumb()
{
    return cpsr & CPSR_T;
}

u32 RegisterBank::operator[](int index) const
{
    return regs[index];
}

u32& RegisterBank::operator[](int index)
{
    return regs[index];
}
