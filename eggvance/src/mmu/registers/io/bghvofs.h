#pragma once

#include "../register.h"

class Bghvofs : public Register<u16>
{
public:
    Bghvofs(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16, 0, 9> offset;  // X / y offset (0..511)
};

using Bghofs = Bghvofs;
using Bgvofs = Bghvofs;
