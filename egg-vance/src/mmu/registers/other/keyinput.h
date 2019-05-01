#pragma once

#include "../register.h"

class Keyinput : public Register
{
public:
    Keyinput(u16& data);

    using Register::operator=;
    using Register::operator unsigned int;

    Field<0, 1> a;       // Button A (0 = pressed)
    Field<1, 1> b;       // Button B (0 = pressed)
    Field<2, 1> select;  // Select (0 = pressed)
    Field<3, 1> start;   // Select (0 = pressed)
    Field<4, 1> right;   // Right (0 = pressed)
    Field<5, 1> left;    // Left (0 = pressed)
    Field<6, 1> up;      // Up (0 = pressed)
    Field<7, 1> down;    // Down (0 = pressed)
    Field<8, 1> r;       // Button R (0 = pressed)
    Field<9, 1> l;       // Button L (0 = pressed)
};

