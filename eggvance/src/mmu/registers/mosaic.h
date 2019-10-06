#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct Mosaic
{
    struct Stretch
    {
        void reset();

        int x;  // Stretch along x-axis (minus 1)
        int y;  // Stretch along y-axis (minus 1)
    };

    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    Stretch bg;   // Background stretch
    Stretch obj;  // Object stretch
};

template<unsigned index>
inline void Mosaic::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        bg.x = bits<0, 4>(byte);
        bg.y = bits<4, 4>(byte);
        break;

    case 1:
        obj.x = bits<0, 4>(byte);
        obj.y = bits<4, 4>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
