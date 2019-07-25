#pragma once

#include "common/bitfield.h"
#include "common/integer.h"

// Todo: make this a separate class
template<typename T>
using Register = BitField<T, 0, 8 * sizeof(T)>;

struct Register16
{
    union
    {
        u8 bytes[2];
        u16 data;
    };
};

struct Register32
{
    union
    {
        u8 bytes[4];
        u32 data;
    };
};
