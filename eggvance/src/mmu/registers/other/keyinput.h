#pragma once

#include "../register.h"

class Keyinput : public Register<u16>
{
public:
    Keyinput(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16, 0, 1> a;       // Button A (0 = pressed)
    BitField<u16, 1, 1> b;       // Button B (0 = pressed)
    BitField<u16, 2, 1> select;  // Select (0 = pressed)
    BitField<u16, 3, 1> start;   // Select (0 = pressed)
    BitField<u16, 4, 1> right;   // Right (0 = pressed)
    BitField<u16, 5, 1> left;    // Left (0 = pressed)
    BitField<u16, 6, 1> up;      // Up (0 = pressed)
    BitField<u16, 7, 1> down;    // Down (0 = pressed)
    BitField<u16, 8, 1> r;       // Button R (0 = pressed)
    BitField<u16, 9, 1> l;       // Button L (0 = pressed)
};

