#pragma once

#include <shell/array.h>

#include "psr.h"

class Registers
{
public:
    Registers();

    union
    {
        struct
        {
            shell::array<u32, 13> gprs;
            u32 sp;
            u32 lr;
            u32 pc;
        };
        shell::array<u32, 16> regs = {};
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
        shell::array<u32, 6, 3> def = {};
        shell::array<u32, 2, 5> fiq = {};
    } bank;
};
