#pragma once

#include "common/integer.h"
#include "enums.h"

class RegisterBank
{
public:
    Mode mode() const;
    void switchMode(Mode mode);

    union
    {
        struct
        {
            u32 r0;
            u32 r1;
            u32 r2;
            u32 r3;
            u32 r4;
            u32 r5;
            u32 r6;
            u32 r7;
            u32 r8;
            u32 r9;
            u32 r10;
            u32 r11;
            u32 r12;

            u32 sp;
            u32 lr;
            u32 pc;
        };
        u32 regs[16];
    };

    u32 cpsr;
    u32 spsr;

protected:
    enum Bank
    {
        BANK_DEF,
        BANK_FIQ,
        BANK_IRQ,
        BANK_SVC,
        BANK_ABT,
        BANK_UND
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
    static Bank modeToBank(Mode mode);
};
