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
        void reset();
        void updateFlags();

        inline void write(u8 byte);

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

        int flags;

    };

    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int mode;     // Blend mode
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
        upper.write(byte);
        upper.updateFlags();
        mode = bits<6, 2>(byte);
        break;

    case 1:
        lower.write(byte);
        lower.updateFlags();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

inline void BlendControl::Layer::write(u8 byte)
{
    bg0 = bits<0, 1>(byte);
    bg1 = bits<1, 1>(byte);
    bg2 = bits<2, 1>(byte);
    bg3 = bits<3, 1>(byte);
    obj = bits<4, 1>(byte);
    bdp = bits<5, 1>(byte);
}
