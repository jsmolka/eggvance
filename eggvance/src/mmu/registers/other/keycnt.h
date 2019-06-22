#pragma once

#include "../register.h"

class Keycnt : public Register<u16>
{
public:
    Keycnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> a;       // Button A (1 = select)
    BitField<u16,  1, 1> b;       // Button B (1 = select)
    BitField<u16,  2, 1> select;  // Select (1 = select)
    BitField<u16,  3, 1> start;   // Select (1 = select)
    BitField<u16,  4, 1> right;   // Right (1 = select)
    BitField<u16,  5, 1> left;    // Left (1 = select)
    BitField<u16,  6, 1> up;      // Up (1 = select)
    BitField<u16,  7, 1> down;    // Down (1 = select)
    BitField<u16,  8, 1> r;       // Button R (1 = select)
    BitField<u16,  9, 1> l;       // Button L (1 = select)
    BitField<u16, 14, 1> irq;     // IRQ enable (1 = enabled)
    BitField<u16, 15, 1> logic;   // IRQ logic (0 = or, 1 = and)
};

