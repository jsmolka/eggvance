#pragma once

#include "../register.h"

class Bgpabcd : public Register<u16>
{
public:
    Bgpabcd(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16,  0, 8> fractal;  // Fractional portion
    BitField<u16,  8, 7> integer;  // Integer portion
    BitField<u16, 15, 1> sign;     // Sign

    float value() const;
};

using Bgpa = Bgpabcd;
using Bgpb = Bgpabcd;
using Bgpc = Bgpabcd;
using Bgpd = Bgpabcd;
