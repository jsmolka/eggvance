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

    u32 bank_all[6][3];
    u32 bank_fiq[2][5];
};
