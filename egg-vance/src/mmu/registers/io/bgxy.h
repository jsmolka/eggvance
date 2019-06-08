#pragma once

#include "../register.h"

class Bgxy : public Register<u32>
{
public:
    Bgxy(u32& data);

    using Register<u32>::operator=;
    using Register<u32>::operator unsigned int;

    BitField<u32,  0,  8> fractal;  // Fractional portion
    BitField<u32,  8, 19> integer;  // Integer portion
    BitField<u32, 27,  1> sign;     // Sign

    float internal;  // Copy register during vblank, increment by pb / pd after scanline

    float value() const;
};

using Bgx = Bgxy;
using Bgy = Bgxy;
