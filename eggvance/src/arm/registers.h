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
        u32 regs[16] = {};
    };

    Psr cpsr;
    Psr spsr;

protected:
    void switchMode(uint mode);

private:
    enum Bank
    {
        kBankDef,
        kBankFiq,
        kBankIrq,
        kBankSvc,
        kBankAbt,
        kBankUnd
    };

    static Bank modeToBank(uint mode);

    u32 bank_def[6][3] = {};
    u32 bank_fiq[2][5] = {};
};
