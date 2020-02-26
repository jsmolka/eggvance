#pragma once

#include "register.h"
#include "common/bits.h"

class WindowDimension : public Register<WindowDimension, 2>
{
public:
    WindowDimension(uint limit)
        : limit(limit) {}

    inline void reset()
    {
        *this = WindowDimension(limit);
    }

    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        max = data[0];
        min = data[1];

        if (max > limit || max < min)
            max = limit;
    }

    inline bool contains(uint x) const
    {
        return x >= min && x < max;
    }

    uint min = 0;
    uint max = 0;

private:
    uint limit;
};
