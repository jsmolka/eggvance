#pragma once

#include "common/macros.h"
#include "common/utility.h"

enum BlendMode
{
    BLD_DISABLED = 0b00,  // Blend disabled
    BLD_ALPHA    = 0b01,  // Blend alpha
    BLD_WHITE    = 0b10,  // Blend with white
    BLD_BLACK    = 0b11   // Blend with black
};

struct BlendControl
{
    struct Layer
    {
        union
        {
            struct
            {
                int bg0;  // BG0 enable
                int bg1;  // BG1 enable
                int bg2;  // BG2 enable
                int bg3;  // BG3 enable
            };
            int bg[4];
        };
        int obj;  // Objects enable
        int bdp;  // Backdrop enable
    };

    template<unsigned index>
    inline void write(u8 byte);

    int mode;     // Mode (0 = none, 1 = alpha blending, 2 = brightness increase, 3 = brightness decrease)
    Layer upper;  // Upper blend layer
    Layer lower;  // Lower blend layer
};

template<unsigned index>
inline void BlendControl::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        upper.bg0 = bits<0, 1>(byte);
        upper.bg1 = bits<1, 1>(byte);
        upper.bg2 = bits<2, 1>(byte);
        upper.bg3 = bits<3, 1>(byte);
        upper.obj = bits<4, 1>(byte);
        upper.bdp = bits<5, 1>(byte);
        mode      = bits<6, 2>(byte);
        break;

    case 1:
        lower.bg0 = bits<0, 1>(byte);
        lower.bg1 = bits<1, 1>(byte);
        lower.bg2 = bits<2, 1>(byte);
        lower.bg3 = bits<3, 1>(byte);
        lower.obj = bits<4, 1>(byte);
        lower.bdp = bits<5, 1>(byte);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
