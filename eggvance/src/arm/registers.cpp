#include "registers.h"

#include <algorithm>

#include "common/macros.h"
#include "common/config.h"

Registers::Registers()
{
    reset();
}

u32 Registers::operator[](int index) const
{
    return regs[index];
}

u32& Registers::operator[](int index)
{
    return regs[index];
}

void Registers::reset()
{
    std::memset(regs,     0, sizeof(regs));
    std::memset(bank,     0, sizeof(bank));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    if (!cfg.skip_bios)
    {
        pc   = 0x0800'0008;
        sp   = 0x0300'7F00;
        cpsr = 0x0000'005F;

        bank[Bank::FIQ].sp = 0x0300'7F00;
        bank[Bank::ABT].sp = 0x0300'7F00;
        bank[Bank::UND].sp = 0x0300'7F00;
        bank[Bank::SVC].sp = 0x0300'7FE0;
        bank[Bank::IRQ].sp = 0x0300'7FA0;
    }
    else 
    {
        pc   = 0x0000'0008;
        cpsr = 0x0000'00D3;
    }
}

void Registers::switchMode(PSR::Mode mode)
{
    Bank old_bank = modeToBank(static_cast<PSR::Mode>(cpsr.mode));
    Bank new_bank = modeToBank(mode);

    if (old_bank != new_bank)
    {
        if (old_bank == Bank::FIQ || new_bank == Bank::FIQ)
        {
            int old_fiq = old_bank == Bank::FIQ;
            int new_fiq = new_bank == Bank::FIQ;

            bank_fiq[old_fiq].r8  = regs[8 ];
            bank_fiq[old_fiq].r9  = regs[9 ];
            bank_fiq[old_fiq].r10 = regs[10];
            bank_fiq[old_fiq].r11 = regs[11];
            bank_fiq[old_fiq].r12 = regs[12];

            regs[8 ] = bank_fiq[new_fiq].r8;
            regs[9 ] = bank_fiq[new_fiq].r9;
            regs[10] = bank_fiq[new_fiq].r10;
            regs[11] = bank_fiq[new_fiq].r11;
            regs[12] = bank_fiq[new_fiq].r12;
        }

        bank[old_bank].sp   = sp;
        bank[old_bank].lr   = lr;
        bank[old_bank].spsr = spsr;

        sp   = bank[new_bank].sp;
        lr   = bank[new_bank].lr;
        spsr = bank[new_bank].spsr;
    }
    cpsr.mode = mode;
}

Registers::Bank Registers::modeToBank(PSR::Mode mode)
{
    switch (mode)
    {
    case PSR::Mode::USR:
    case PSR::Mode::SYS: return Bank::DEF;
    case PSR::Mode::FIQ: return Bank::FIQ;
    case PSR::Mode::SVC: return Bank::SVC;
    case PSR::Mode::ABT: return Bank::ABT;
    case PSR::Mode::IRQ: return Bank::IRQ;
    case PSR::Mode::UND: return Bank::UND;

    default:
        EGG_UNREACHABLE;
        return Bank::DEF;
    }
}
