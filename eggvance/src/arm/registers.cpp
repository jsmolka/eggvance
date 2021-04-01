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
        cpsr = 0x0000'001F;

        bank.def[uint(Bank::Def)][0] = 0x0300'7F00;
        bank.def[uint(Bank::Def)][1] = 0x0000'00C0;
        bank.def[uint(Bank::Irq)][0] = 0x0300'7FA0;
        bank.def[uint(Bank::Svc)][0] = 0x0300'7FE0;
    }
    else 
    {
        spsr = 0x0000'0000;
        cpsr = 0x0000'00D3;
    }
}

void Registers::switchMode(Psr::Mode mode)
{
    Bank bank_old = modeToBank(cpsr.m);
    Bank bank_new = modeToBank(mode);

    if (bank_old != bank_new)
    {
        bank.def[uint(bank_old)][0] = sp;
        bank.def[uint(bank_old)][1] = lr;
        bank.def[uint(bank_old)][2] = spsr;

        sp   = bank.def[uint(bank_new)][0];
        lr   = bank.def[uint(bank_new)][1];
        spsr = bank.def[uint(bank_new)][2];

        if (bank_old == Bank::Fiq || bank_new == Bank::Fiq)
        {
            uint fiq_old = bank_old == Bank::Fiq;
            uint fiq_new = bank_new == Bank::Fiq;

            bank.fiq[fiq_old][0] = regs[ 8];
            bank.fiq[fiq_old][1] = regs[ 9];
            bank.fiq[fiq_old][2] = regs[10];
            bank.fiq[fiq_old][3] = regs[11];
            bank.fiq[fiq_old][4] = regs[12];

            regs[ 8] = bank.fiq[fiq_new][0];
            regs[ 9] = bank.fiq[fiq_new][1];
            regs[10] = bank.fiq[fiq_new][2];
            regs[11] = bank.fiq[fiq_new][3];
            regs[12] = bank.fiq[fiq_new][4];
        }
    }
    cpsr.m = mode;
}

Registers::Bank Registers::modeToBank(Psr::Mode mode)
{
    switch (mode)
    {
    case Psr::Mode::Usr: return Bank::Def;
    case Psr::Mode::Sys: return Bank::Def;
    case Psr::Mode::Fiq: return Bank::Fiq;
    case Psr::Mode::Irq: return Bank::Irq;
    case Psr::Mode::Svc: return Bank::Svc;
    case Psr::Mode::Abt: return Bank::Abt;
    case Psr::Mode::Und: return Bank::Und;
    }
    return Bank::Def;
}
