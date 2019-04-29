#pragma once

#include "../field.h"

class Vcount
{
public:
    Vcount(u16& data);

    u16& data;

    Field<0, 8> ly;
};

