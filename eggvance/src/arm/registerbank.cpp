#include "registerbank.h"

Mode RegisterBank::mode() const
{
    return static_cast<Mode>(cpsr.mode);
}

void RegisterBank::switchMode(Mode mode)
{
    Bank old_bank = modeToBank(this->mode());
    Bank new_bank = modeToBank(mode);

    if (old_bank != new_bank)
    {
        if (old_bank == BANK_FIQ || new_bank == BANK_FIQ)
        {
            int old_fiq = old_bank == BANK_FIQ;
            int new_fiq = new_bank == BANK_FIQ;

            bank_fiq[old_fiq].r8  = r8;
            bank_fiq[old_fiq].r9  = r9;
            bank_fiq[old_fiq].r10 = r10;
            bank_fiq[old_fiq].r11 = r11;
            bank_fiq[old_fiq].r12 = r12;

            r8  = bank_fiq[new_fiq].r8;
            r9  = bank_fiq[new_fiq].r9;
            r10 = bank_fiq[new_fiq].r10;
            r11 = bank_fiq[new_fiq].r11;
            r12 = bank_fiq[new_fiq].r12;
        }

        bank[old_bank].sp   = sp;
        bank[old_bank].lr   = lr;
        bank[old_bank].spsr = spsr;

        sp   = bank[new_bank].sp;
        lr   = bank[new_bank].lr;
        spsr = bank[new_bank].spsr;
    }
    cpsr = (cpsr & ~0x1F) | mode;
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
    }
    return BANK_DEF;
}
