#pragma once

#include "../register.h"

class Bgvofs : public Register<u16>
{
public:
    Bgvofs(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16, 0, 8> offset;  // Y offset (0..511)
};
