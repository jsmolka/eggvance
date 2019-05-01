#pragma once

#include "../register.h"

class Vcount : public Register
{
public:
    Vcount(u16& data);

    using Register::operator=;
    using Register::operator unsigned int;

    Field<0, 8> line;  // Current scanline (0..227)
};
