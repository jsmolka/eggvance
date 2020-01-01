#include "registers.h"

#include <cstring>

#include "common/macros.h"
#include "common/config.h"

void Registers::reset()
{
    for (auto& reg : regs)
    {
        reg.reset();
    }

    std::memset(bank_all, 0, sizeof(bank_all));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    if (config.bios_skip)
    {
        sp   = 0x0300'7F00;
        lr   = 0x0800'0000;
        pc   = 0x0800'0000;
        cpsr = 0x0000'005F;

        bank_all[Bank::FIQ][0] = 0x0300'7F00;
        bank_all[Bank::ABT][0] = 0x0300'7F00;
        bank_all[Bank::UND][0] = 0x0300'7F00;
        bank_all[Bank::SVC][0] = 0x0300'7FE0;
        bank_all[Bank::IRQ][0] = 0x0300'7FA0;
    }
    else 
    {
        pc   = 0x0000'0000;
        cpsr = 0x0000'00D3;
    }
}

void Registers::switchMode(PSR::Mode mode)
{
    Bank bank_old = modeToBank(cpsr.mode);
    Bank bank_new = modeToBank(mode);

    if (bank_old != bank_new)
    {
        bank_all[bank_old][0] = sp;
        bank_all[bank_old][1] = lr;
        bank_all[bank_old][2] = spsr;

        sp   = bank_all[bank_new][0];
        lr   = bank_all[bank_new][1];
        spsr = bank_all[bank_new][2];

        if (bank_old == Bank::FIQ || bank_new == Bank::FIQ)
        {
            int fiq_old = bank_old == Bank::FIQ;
            int fiq_new = bank_new == Bank::FIQ;

            bank_fiq[fiq_old][0] = regs[ 8];
            bank_fiq[fiq_old][1] = regs[ 9];
            bank_fiq[fiq_old][2] = regs[10];
            bank_fiq[fiq_old][3] = regs[11];
            bank_fiq[fiq_old][4] = regs[12];

            regs[ 8] = bank_fiq[fiq_new][0];
            regs[ 9] = bank_fiq[fiq_new][1];
            regs[10] = bank_fiq[fiq_new][2];
            regs[11] = bank_fiq[fiq_new][3];
            regs[12] = bank_fiq[fiq_new][4];
        }
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
    case PSR::Mode::IRQ: return Bank::IRQ;
    case PSR::Mode::SVC: return Bank::SVC;
    case PSR::Mode::ABT: return Bank::ABT;
    case PSR::Mode::UND: return Bank::UND;

    default:
        EGG_UNREACHABLE;
        return Bank::DEF;
    }
}
