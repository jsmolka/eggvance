#pragma once

#include "common/integer.h"

template<s16 initial>
class BGParameter
{
public:
    inline operator s16() const
    {
        return value;
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&value)[index] = byte;
    }

    s16 value = initial;
};

using BGParameterA = BGParameter<0x0100>;
using BGParameterB = BGParameter<0x0000>;
using BGParameterC = BGParameter<0x0000>;
using BGParameterD = BGParameter<0x0100>;
