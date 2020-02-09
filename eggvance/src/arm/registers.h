#pragma once

#include "psr.h"

class Registers
{
public:
    void reset();

    union
    {
        struct
        {
            u32 gprs[13];
            u32 sp;
            u32 lr;
            u32 pc;
        };
        u32 regs[16];
    };

    PSR cpsr;
    PSR spsr;

protected:
    void switchMode(PSR::Mode mode);

private:
    enum Bank
    {
        BANK_DEF = 0,
        BANK_FIQ = 1,
        BANK_IRQ = 2,
        BANK_SVC = 3,
        BANK_ABT = 4,
        BANK_UND = 5
    };

    static Bank modeToBank(PSR::Mode mode);

    u32 bank_all[6][3];
    u32 bank_fiq[2][5];
};
