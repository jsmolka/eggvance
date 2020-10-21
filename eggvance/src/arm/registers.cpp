#include "registers.h"

#include "base/config.h"

Registers::Registers()
{
    if (config.bios_skip)
    {
        sp   = 0x0300'7F00;
        lr   = 0x0800'0000;
        pc   = 0x0800'0000;
        spsr = 0x0000'0000;
        cpsr = 0x0000'005F;

        bank_def[kBankFiq][0] = 0x0300'7F00;
        bank_def[kBankAbt][0] = 0x0300'7F00;
        bank_def[kBankUnd][0] = 0x0300'7F00;
        bank_def[kBankSvc][0] = 0x0300'7FE0;
        bank_def[kBankIrq][0] = 0x0300'7FA0;
    }
    else 
    {
        spsr = 0x0000'0000;
        cpsr = 0x0000'00D3;
    }
}

void Registers::switchMode(uint mode)
{
    Bank bank_old = modeToBank(cpsr.m);
    Bank bank_new = modeToBank(mode);

    if (bank_old != bank_new)
    {
        bank_def[bank_old][0] = sp;
        bank_def[bank_old][1] = lr;
        bank_def[bank_old][2] = spsr;

        sp   = bank_def[bank_new][0];
        lr   = bank_def[bank_new][1];
        spsr = bank_def[bank_new][2];

        if (bank_old == kBankFiq || bank_new == kBankFiq)
        {
            uint fiq_old = bank_old == kBankFiq;
            uint fiq_new = bank_new == kBankFiq;

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

Registers::Bank Registers::modeToBank(uint mode)
{
    switch (mode)
    {
    case Psr::kModeUsr: return kBankDef;
    case Psr::kModeSys: return kBankDef;
    case Psr::kModeFiq: return kBankFiq;
    case Psr::kModeIrq: return kBankIrq;
    case Psr::kModeSvc: return kBankSvc;
    case Psr::kModeAbt: return kBankAbt;
    case Psr::kModeUnd: return kBankUnd;
    }
    return kBankDef;
}
