#pragma once

#include <algorithm>

#include "register.h"
#include "common/bits.h"

class BlendAlpha : public TRegister<BlendAlpha, 2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            eva = std::min(16u, bits<0, 5, uint>(byte));
        else
            evb = std::min(16u, bits<0, 5, uint>(byte));

        data[index] = byte;
    }

    inline u16 blend(u16 a, u16 b) const
    {
        uint ar = bits< 0, 5>(a);
        uint ag = bits< 5, 5>(a);
        uint ab = bits<10, 5>(a);

        uint br = bits< 0, 5>(b);
        uint bg = bits< 5, 5>(b);
        uint bb = bits<10, 5>(b);

        uint tr = std::min(31u, (ar * eva + br * evb) >> 4);
        uint tg = std::min(31u, (ag * eva + bg * evb) >> 4);
        uint tb = std::min(31u, (ab * eva + bb * evb) >> 4);

        return (tr << 0) | (tg << 5) | (tb << 10);
    }

private:
    uint eva = 0;
    uint evb = 0;
};
