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
        DEF = 0,
        FIQ = 1,
        IRQ = 2,
        SVC = 3,
        ABT = 4,
        UND = 5
    };

    static Bank modeToBank(PSR::Mode mode);

    u32 bank[6][3];
    u32 bank_fiq[2][5];
};
