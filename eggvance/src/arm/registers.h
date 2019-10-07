#pragma once

#include "psr.h"

class Registers
{
public:
    Registers();

    u32  operator[](int index) const;
    u32& operator[](int index);

    void reset();

    void switchMode(PSR::Mode mode);

    union
    {
        struct
        {
            u32 gpr[13];  // General Purpose Registers
            u32 sp;       // Stack Pointer
            u32 lr;       // Link Register
            u32 pc;       // Program Counter
        };
        u32 regs[16];
    };

    PSR cpsr;
    PSR spsr;

protected:
    enum Bank
    {
        DEF = 0,
        FIQ = 1,
        IRQ = 2,
        SVC = 3,
        ABT = 4,
        UND = 5
    };

    struct
    {
        u32 sp;
        u32 lr;
        u32 spsr;
    } bank[6];

    struct
    {
        u32 r8;
        u32 r9;
        u32 r10;
        u32 r11;
        u32 r12;
    } bank_fiq[2];

private:
    static Bank modeToBank(PSR::Mode mode);
};
