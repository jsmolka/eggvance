#pragma once

struct Mosaic
{
    struct Stretch
    {
        int x;  // Stretch along x-axis (minus 1)
        int y;  // Stretch along y-axis (minus 1)
    };

    Stretch bg;   // Background stretch
    Stretch obj;  // Object stretch
};
