#pragma once

#include "psr.h"

class Registers
{
public:
    u32  operator[](int index) const;
    u32& operator[](int index);

    void reset();

    void switchMode(PSR::Mode mode);

    union
    {
        struct
        {
            u32 gprs[13];  // General Purpose Registers
            u32 sp;        // Stack Pointer
            u32 lr;        // Link Register
            u32 pc;        // Program Counter
        };
        u32 regs[16];
    };

    PSR cpsr;  // Current Program Status Register
    PSR spsr;  // Saved Program Status Register

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
