#pragma once

#include "gpr.h"
#include "psr.h"

class Registers
{
public:
    Registers() : regs{} {};
    ~Registers() {};

    void reset();

    union
    {
        struct
        {
            GPR gprs[13];
            GPR sp;
            GPR lr;
            GPR pc;
        };
        GPR regs[16];
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
