#pragma once

#include "common/integer.h"

class PSR
{
public:
    enum class Condition
    {
        EQ = 0x0,
        NE = 0x1,
        CS = 0x2,
        CC = 0x3,
        MI = 0x4,
        PL = 0x5,
        VS = 0x6,
        VC = 0x7,
        HI = 0x8,
        LS = 0x9,
        GE = 0xA,
        LT = 0xB,
        GT = 0xC,
        LE = 0xD,
        AL = 0xE,
        NV = 0xF,
    };

    enum class Mode : u32
    {
        USR = 0b10000,
        FIQ = 0b10001,
        IRQ = 0b10010,
        SVC = 0b10011,
        ABT = 0b10111,
        SYS = 0b11111,
        UND = 0b11011
    };

    PSR& operator=(const PSR& other);
    PSR& operator=(u32 value);
    operator u32() const;

    bool check(Condition condition) const;

    union
    {
        struct
        {
            Mode mode   :  5;
            u32  thumb  :  1;
            u32  fiqd   :  1;
            u32  irqd   :  1;
            u32  unused : 20;
            u32  v      :  1;
            u32  c      :  1;
            u32  z      :  1;
            u32  n      :  1;
        };
        u32 value;
    };
};
