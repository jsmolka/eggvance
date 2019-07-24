#pragma once

#include "common/integer.h"

enum Mode
{
    MODE_USR = 0b10000,  // User
    MODE_FIQ = 0b10001,  // Fast interrupt request
    MODE_IRQ = 0b10010,  // Interrupt request
    MODE_SVC = 0b10011,  // Supervisor
    MODE_ABT = 0b10111,  // Abort
    MODE_SYS = 0b11111,  // System
    MODE_UND = 0b11011   // Undefined
};

enum CPSR
{
    CPSR_M = 0x1F,     // Mode
    CPSR_T = 1 <<  5,  // THUMB enable
    CPSR_F = 1 <<  6,  // FIQ disable
    CPSR_I = 1 <<  7,  // IRQ disable
    CPSR_V = 1 << 28,  // Overflow
    CPSR_C = 1 << 29,  // Carry
    CPSR_Z = 1 << 30,  // Zero
    CPSR_N = 1 << 31   // Negative
};

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
