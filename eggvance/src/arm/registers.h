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
    void switchMode(Psr::Mode mode);

private:
    enum class Bank { Def, Fiq, Irq, Svc, Abt, Und };

    static Bank modeToBank(Psr::Mode mode);

    struct
    {
        u32 def[6][3] = {};
        u32 fiq[2][5] = {};
    } banks;
};
