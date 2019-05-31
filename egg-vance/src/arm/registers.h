#pragma once

#include "registerbank.h"
#include "common/bitfield.h"

class Registers : public RegisterBank
{
public:
    Registers();

    void reset();

    BitField<u32,  5, 1> thumb;
    BitField<u32,  6, 1> fiq_disable;
    BitField<u32,  7, 1> irq_disable;
    BitField<u32, 28, 1> v;
    BitField<u32, 29, 1> c;
    BitField<u32, 30, 1> z;
    BitField<u32, 31, 1> n;

    bool check(Condition condition) const;

    u32  operator[](int index) const;
    u32& operator[](int index);
};

