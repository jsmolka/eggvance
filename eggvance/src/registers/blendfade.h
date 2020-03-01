#pragma once

#include "common/bits.h"

class BlendFade
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            evy = std::min(16u, bits<0, 5, uint>(byte));
    }

    inline u16 blendWhite(u16 a) const
    {
        constexpr uint rmask = 0x1F <<  0;
        constexpr uint gmask = 0x1F <<  5;
        constexpr uint bmask = 0x1F << 10;

        uint r = a & rmask;
        uint g = a & gmask;
        uint b = a & bmask;

        r += ((rmask - r) * evy) >> 4;
        g += ((gmask - g) * evy) >> 4;
        b += ((bmask - b) * evy) >> 4;

        return (r & rmask) | (g & gmask) | (b & bmask);
    }

    inline u16 blendBlack(u16 a) const
    {
        constexpr uint rmask = 0x1F <<  0;
        constexpr uint gmask = 0x1F <<  5;
        constexpr uint bmask = 0x1F << 10;

        uint r = a & rmask;
        uint g = a & gmask;
        uint b = a & bmask;

        r -= (r * evy) >> 4;
        g -= (g * evy) >> 4;
        b -= (b * evy) >> 4;

        return (r & rmask) | (g & gmask) | (b & bmask);
    }

private:
    uint evy = 0;
};
