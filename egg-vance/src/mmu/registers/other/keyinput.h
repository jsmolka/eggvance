#pragma once

#include "../field.h"

class Keyinput
{
public:
    Keyinput(u16& data);

    u16& data;

    Field<0, 1> a;
    Field<1, 1> b;
    Field<2, 1> select;
    Field<3, 1> start;
    Field<4, 1> right;
    Field<5, 1> left;
    Field<6, 1> up;
    Field<7, 1> down;
    Field<8, 1> r;
    Field<9, 1> l;
};

