#include "registers.h"

#include <cstring>

#include "common/config.h"

void Registers::reset()
{
    std::memset(regs    , 0, sizeof(regs));
    std::memset(bank_all, 0, sizeof(bank_all));
    std::memset(bank_fiq, 0, sizeof(bank_fiq));

    if (config.bios_skip)
    {
        sp   = 0x0300'7F00;
        lr   = 0x0800'0000;
        pc   = 0x0800'0000;
        spsr = 0x0000'0000;
        cpsr = 0x0000'005F;

        bank_all[BANK_FIQ][0] = 0x0300'7F00;
        bank_all[BANK_ABT][0] = 0x0300'7F00;
        bank_all[BANK_UND][0] = 0x0300'7F00;
        bank_all[BANK_SVC][0] = 0x0300'7FE0;
        bank_all[BANK_IRQ][0] = 0x0300'7FA0;
    }
    else 
    {
        spsr = 0x0000'0000;
        cpsr = 0x0000'00D3;
    }
}

void Registers::switchMode(PSR::Mode mode)
{
    Bank bank_old = modeToBank(cpsr.m);
    Bank bank_new = modeToBank(mode);

    if (bank_old != bank_new)
    {
        bank_all[bank_old][0] = sp;
        bank_all[bank_old][1] = lr;
        bank_all[bank_old][2] = spsr;

        sp   = bank_all[bank_new][0];
        lr   = bank_all[bank_new][1];
        spsr = bank_all[bank_new][2];

        if (bank_old == BANK_FIQ || bank_new == BANK_FIQ)
        {
            uint fiq_old = static_cast<uint>(bank_old == BANK_FIQ);
            uint fiq_new = static_cast<uint>(bank_new == BANK_FIQ);

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
    cpsr.m = mode;
}

Registers::Bank Registers::modeToBank(PSR::Mode mode)
{
    switch (mode)
    {
    case PSR::Mode::Usr: return BANK_DEF;
    case PSR::Mode::Sys: return BANK_DEF;
    case PSR::Mode::Fiq: return BANK_FIQ;
    case PSR::Mode::Irq: return BANK_IRQ;
    case PSR::Mode::Svc: return BANK_SVC;
    case PSR::Mode::Abt: return BANK_ABT;
    case PSR::Mode::Und: return BANK_UND;

    default:
        UNREACHABLE;
        return BANK_DEF;
    }
}
