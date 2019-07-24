#pragma once

#include "common/bitfield.h"
#include "registerbank.h"

enum class Condition
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

class Registers : public RegisterBank
{
public:
    Registers();

    void reset();

    BitField<u32,  5, 1> thumb;
    BitField<u32,  6, 1> fiqd;
    BitField<u32,  7, 1> irqd;
    BitField<u32, 28, 1> v;
    BitField<u32, 29, 1> c;
    BitField<u32, 30, 1> z;
    BitField<u32, 31, 1> n;

    bool check(Condition condition) const;

    u32  operator[](int index) const;
    u32& operator[](int index);
};

