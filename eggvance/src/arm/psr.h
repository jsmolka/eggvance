#pragma once

#include "common/integer.h"

struct PSR
{
    enum Condition
    {
        EQ = 0x0,  // Equal
        NE = 0x1,  // Not equal
        CS = 0x2,  // Carry set
        CC = 0x3,  // Carry clear
        MI = 0x4,  // Minus
        PL = 0x5,  // Plus
        VS = 0x6,  // V set
        VC = 0x7,  // V clear
        HI = 0x8,  // Unsigned higher
        LS = 0x9,  // Unsigned lower or same
        GE = 0xA,  // Greater or equal
        LT = 0xB,  // Less than
        GT = 0xC,  // Greater than
        LE = 0xD,  // Less or equal
        AL = 0xE,  // Always
        NV = 0xF,  // Never
    };

    enum Mode : u32
    {
        USR = 0b10000,  // User
        FIQ = 0b10001,  // Fast interrupt request
        IRQ = 0b10010,  // Interrupt request
        SVC = 0b10011,  // Supervisor
        ABT = 0b10111,  // Abort
        SYS = 0b11111,  // System
        UND = 0b11011   // Undefined
    };

    PSR& operator=(const PSR& other);
    PSR& operator=(u32 value);
    operator u32() const;

    bool check(Condition condition) const;

    union
    {
        struct
        {
            Mode mode   :  5;  // Mode
            u32  thumb  :  1;  // Thumb enable
            u32  fiqd   :  1;  // FIQ disable
            u32  irqd   :  1;  // IRQ disable
            u32  unused : 20;  // Reserved
            u32  v      :  1;  // Overflow
            u32  c      :  1;  // Carry
            u32  z      :  1;  // Zero
            u32  n      :  1;  // Negative
        };
        u32 value;
    };
};
