#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct DisplayControl
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int mode;         // BG video mode (6,7 prohibited)
    int frame;        // Frame used in BG modes 4,5
    int oam_hblank;   // Allow OAM access during H-Blank
    int mapping_1d;   // Object character mapping (0 = 2D, 1 = 1D)
    int force_blank;  // Forced blank (1 = allow fast access to VRAM, palette and OAM)
    union
    {
        struct
        {
            int bg0;  // Display BG0 flag
            int bg1;  // Display BG1 flag
            int bg2;  // Display BG2 flag
            int bg3;  // Display BG3 flag
        };
        int bg[4];
    };
    int obj;     // Display objects flag
    int win0;    // Display window 0 flag
    int win1;    // Display window 1 flag
    int winobj;  // Display object window flag
};

template<unsigned index>
inline void DisplayControl::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        mode        = bits<0, 3>(byte);
        frame       = bits<4, 1>(byte);
        oam_hblank  = bits<5, 1>(byte);
        mapping_1d  = bits<6, 1>(byte);
        force_blank = bits<7, 1>(byte);
        break;

    case 1:
        bg0    = bits<0, 1>(byte);
        bg1    = bits<1, 1>(byte);
        bg2    = bits<2, 1>(byte);
        bg3    = bits<3, 1>(byte);
        obj    = bits<4, 1>(byte);
        win0   = bits<5, 1>(byte);
        win1   = bits<6, 1>(byte);
        winobj = bits<7, 1>(byte);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
