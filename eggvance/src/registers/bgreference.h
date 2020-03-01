#pragma once

#include "common/bits.h"

class BGReference
{
public:
    inline operator s32() const
    {
        return current;
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 4);

        if (index == 3)
            byte = signExtend<4>(byte);

        reinterpret_cast<u8*>(&initial)[index] = byte;

        current = initial;
    }

    inline void hblank(s16 parameter)
    {
        current += parameter;
    }

    inline void vblank()
    {
        current = initial;
    }

    s32 initial = 0;
    s32 current = 0;
};
