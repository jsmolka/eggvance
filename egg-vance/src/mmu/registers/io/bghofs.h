#pragma once

#include "../register.h"

class Bghofs : public Register<u16>
{
public:
    Bghofs(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16, 0, 8> offset;  // X offset (0..511)
};
