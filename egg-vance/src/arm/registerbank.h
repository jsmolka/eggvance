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
            // General Purpose Registers
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

            u32 sp;  // r13 - Stack Pointer
            u32 lr;  // r14 - Link Register
            u32 pc;  // r15 - Program Counter
        };
        u32 regs[16];
    };

    u32 cpsr;  // Current Program Status Register
    u32* spsr;  // Saved Program Status Register

protected:
    enum Bank
    {
        BANK_FIQ = 0,
        BANK_IRQ = 1,
        BANK_SVC = 2,
        BANK_ABT = 3,
        BANK_UND = 4, 
        BANK_DEF = 5  // Bank for user and system modes
    };

    // All modes bank SP and LR
    struct
    {
        u32 sp;
        u32 lr;
    } bank[6];

    // FIQ mode also banks registers 8-12
    struct
    {
        u32 r8;
        u32 r9;
        u32 r10;
        u32 r11;
        u32 r12;
    } bank_fiq[2];

    // Privileged modes save the previous CPSR
    u32 bank_spsr[5];

private:
    static Bank modeToBank(Mode mode);
};
