#pragma once

#include "../register.h"

class Bldalpha : public Register<u16>
{
public:
    Bldalpha(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16, 0, 5> eva;  // EVA coefficient for layer A (0..16 = 0/16, 17..31 = 16/16)
    BitField<u16, 8, 5> evb;  // EVA coefficient for layer B (0..16 = 0/16, 17..31 = 16/16)
};
