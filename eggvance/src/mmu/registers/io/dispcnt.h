#pragma once

#include "../register.h"

class Dispcnt : public Register<u16>
{
public:
    Dispcnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 3> bg_mode;      // BG mode
    BitField<u16,  3, 1> gbc_mode;     // GBC flag
    BitField<u16,  4, 1> frame;        // Frame used in BG modes 4, 5
    BitField<u16,  5, 1> access_oam;   // Allow access to OAM during V-Blank flag
    BitField<u16,  6, 1> mapping_1d;   // Sprite character mapping (0 = 2D, 1 = 1D)
    BitField<u16,  7, 1> force_blank;  // Forced blank (1 = allow fast access to VRAM, palette and OAM)
    BitField<u16,  8, 1> bg0;          // Display BG0 flag
    BitField<u16,  9, 1> bg1;          // Display BG1 flag
    BitField<u16, 10, 1> bg2;          // Display BG2 flag
    BitField<u16, 11, 1> bg3;          // Display BG3 flag
    BitField<u16, 12, 1> obj;          // Display objects flag
    BitField<u16, 13, 1> win0;         // Display window 0 flag
    BitField<u16, 14, 1> win1;         // Display window 1 flag
    BitField<u16, 15, 1> winobj;       // Display OBJ window flag

    int bg(int bg) const;

    u32 frameAddr() const;
};
