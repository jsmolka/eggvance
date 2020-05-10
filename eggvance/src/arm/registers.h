#pragma once

#include "psr.h"

class Registers
{
public:
    Registers();

    union
    {
        struct
        {
            u32 gprs[13];
            u32 sp;
            u32 lr;
            u32 pc;
        };
        u32 regs[16] = { 0 };
    };

    PSR cpsr;
    PSR spsr;

protected:
    void switchMode(uint mode);

private:
    enum Bank
    {
        kBankDef = 0,
        kBankFiq = 1,
        kBankIrq = 2,
        kBankSvc = 3,
        kBankAbt = 4,
        kBankUnd = 5
    };

    static Bank modeToBank(uint mode);

    u32 bank_all[6][3] = { 0 };
    u32 bank_fiq[2][5] = { 0 };
};
