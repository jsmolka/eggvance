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

        banks.def[uint(Bank::Def)][0] = 0x0300'7F00;
        banks.def[uint(Bank::Def)][1] = 0x0000'00C0;
        banks.def[uint(Bank::Irq)][0] = 0x0300'7FA0;
        banks.def[uint(Bank::Svc)][0] = 0x0300'7FE0;
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
        banks.def[uint(bank_old)][0] = sp;
        banks.def[uint(bank_old)][1] = lr;
        banks.def[uint(bank_old)][2] = spsr;

        sp   = banks.def[uint(bank_new)][0];
        lr   = banks.def[uint(bank_new)][1];
        spsr = banks.def[uint(bank_new)][2];

        if (bank_old == Bank::Fiq || bank_new == Bank::Fiq)
        {
            uint fiq_old = bank_old == Bank::Fiq;
            uint fiq_new = bank_new == Bank::Fiq;

            banks.fiq[fiq_old][0] = regs[ 8];
            banks.fiq[fiq_old][1] = regs[ 9];
            banks.fiq[fiq_old][2] = regs[10];
            banks.fiq[fiq_old][3] = regs[11];
            banks.fiq[fiq_old][4] = regs[12];

            regs[ 8] = banks.fiq[fiq_new][0];
            regs[ 9] = banks.fiq[fiq_new][1];
            regs[10] = banks.fiq[fiq_new][2];
            regs[11] = banks.fiq[fiq_new][3];
            regs[12] = banks.fiq[fiq_new][4];
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
