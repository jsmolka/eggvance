#pragma once

struct DisplayControl
{
    int mode;         // BG mode (6-7 prohibited)
    int gbc;          // Game Boy Color mode
    int frame;        // Frame used in BG modes 4, 5
    int oam_hblank;   // Allow OAM access during H-Blank
    int mapping_1d;   // Sprite character mapping (0 = 2D, 1 = 1D)
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

    int obj;          // Display objects flag
    int win0;         // Display window 0 flag
    int win1;         // Display window 1 flag
    int winobj;       // Display OBJ window flag
};
