#pragma once

#include "../register.h"

class Bldy : public Register<u16>
{
public:
    Bldy(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16, 0, 5> evy;  // EVY coefficient for brightness (0..16 = 0/16, 17..31 = 16/16)
};
