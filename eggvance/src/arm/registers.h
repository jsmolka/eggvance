#pragma once

#include "psr.h"
#include "base/array.h"

class Registers
{
public:
    Registers();

    union
    {
        struct
        {
            array<u32, 13> gprs;
            u32 sp;
            u32 lr;
            u32 pc;
        };
        array<u32, 16> regs = {};
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
        array<u32, 6, 3> def = {};
        array<u32, 2, 5> fiq = {};
    } bank;
};
