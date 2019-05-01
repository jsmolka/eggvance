#pragma once

#include "../register.h"

class Dispcnt : public Register
{
public:
    Dispcnt(u16& data);

    Field< 0, 3> bg_mode;      // BG mode (0-5 = video mode 0-5, 6-7 prohibited)
    Field< 3, 1> gbc_mode;     // GBC mode (0 = GBA, 1 = GBC)
    Field< 4, 1> frame;        // Frame used in BG modes 4,5 (0-1 = Frame 0-1)
    Field< 5, 1> access_oam;   // Allow access to OAM during V-Blank (1 = enabled)
    Field< 6, 1> obj_mapping;  // OBJ character VRAM mapping (0 = 2D, 1 = 1D)
    Field< 7, 1> force_blank;  // Forced blank (1 = allow fast access to VRAM, palette and OAM)
    Field< 8, 1> bg0;          // Display BG0 (1 = on)
    Field< 9, 1> bg1;          // Display BG1 (1 = on)
    Field<10, 1> bg2;          // Display BG2 (1 = on)
    Field<11, 1> bg3;          // Display BG3 (1 = on)
    Field<12, 1> obj;          // Display OBJ (1 = on)
    Field<13, 1> win0;         // Display window 0 (1 = on)
    Field<14, 1> win1;         // Display window 1 (1 = on)
    Field<15, 1> winobj;       // Display OBJ window (1 = on)

    u32 frameAddr() const;
};
