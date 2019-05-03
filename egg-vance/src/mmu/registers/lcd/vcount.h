#pragma once

#include "../register.h"

class Vcount : public Register<u16>
{
public:
    Vcount(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    Field<u16, 0, 8> line;  // Current scanline (0..227)
};
