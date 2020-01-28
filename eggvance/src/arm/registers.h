#pragma once

#include "gpr.h"
#include "psr.h"

class Registers
{
public:
    void reset();

    GPR regs[16];
    PSR cpsr;
    PSR spsr;

    GPR& sp = regs[13];
    GPR& lr = regs[14];
    GPR& pc = regs[15];

protected:
    void switchMode(uint mode);

private:
    enum Bank : uint
    {
        kBankDef = 0,
        kBankFiq = 1,
        kBankIrq = 2,
        kBankSvc = 3,
        kBankAbt = 4,
        kBankUnd = 5
    };

    static Bank modeToBank(uint mode);

    u32 bank_all[6][3];
    u32 bank_fiq[2][5];
};
