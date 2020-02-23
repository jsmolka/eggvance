#pragma once

#include "register.h"

class BlendFade : public TRegister<BlendFade, 2>
{
public:
    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            evy = std::min(16u, bits<0, 5, uint>(byte));
    }

    inline u16 blendWhite(u16 a) const
    {
        uint ar = bits< 0, 5>(a);
        uint ag = bits< 5, 5>(a);
        uint ab = bits<10, 5>(a);

        uint tr = std::min(31u, ar + (((31 - ar) * evy) >> 4));
        uint tg = std::min(31u, ag + (((31 - ag) * evy) >> 4));
        uint tb = std::min(31u, ab + (((31 - ab) * evy) >> 4));

        return (tr << 0) | (tg << 5) | (tb << 10);
    }

    inline u16 blendBlack(u16 a) const
    {
        uint ar = bits< 0, 5>(a);
        uint ag = bits< 5, 5>(a);
        uint ab = bits<10, 5>(a);

        uint tr = std::min(31u, ar - ((ar * evy) >> 4));
        uint tg = std::min(31u, ag - ((ag * evy) >> 4));
        uint tb = std::min(31u, ab - ((ab * evy) >> 4));

        return (tr << 0) | (tg << 5) | (tb << 10);
    }

private:
    uint evy = 0;
};
