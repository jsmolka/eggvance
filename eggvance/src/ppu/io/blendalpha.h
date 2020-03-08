#pragma once

#include <algorithm>

#include "common/bits.h"
#include "common/register.h"

class BlendAlpha : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
            eva = std::min(16u, bits<0, 5, uint>(byte));
        else
            evb = std::min(16u, bits<0, 5, uint>(byte));
    }

    inline u16 blendAlpha(u16 a, u16 b) const
    {
        constexpr uint rmask = 0x1F <<  0;
        constexpr uint gmask = 0x1F <<  5;
        constexpr uint bmask = 0x1F << 10;

        uint cr = std::min(rmask, ((a & rmask) * eva + (b & rmask) * evb) >> 4);
        uint cg = std::min(gmask, ((a & gmask) * eva + (b & gmask) * evb) >> 4);
        uint cb = std::min(bmask, ((a & bmask) * eva + (b & bmask) * evb) >> 4);

        return (cr & rmask) | (cg & gmask) | (cb & bmask);
    }

    uint eva = 0;
    uint evb = 0;
};
