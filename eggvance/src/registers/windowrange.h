#pragma once

#include "register.h"
#include "common/constants.h"

template<uint maximum>
class WindowRange : public Register<WindowRange<maximum>, 2>
{
public:
    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        this->data[index] = byte;

        max = this->data[0];
        min = this->data[1];

        if (max > maximum || max < min)
            max = maximum;
    }

    inline bool contains(uint value) const
    {
        return value >= min && value < max;
    }

    uint min = 0;
    uint max = 0;
};

using WindowRangeHor = WindowRange<SCREEN_W>;
using WindowRangeVer = WindowRange<SCREEN_H>;
