#pragma once

#include "common/integer.h"

struct Mosaic
{
    struct Stretch
    {
        void write(u8 byte);

        int x;  // Stretch along x-axis (minus 1)
        int y;  // Stretch along y-axis (minus 1)
    };

    void write(int index, u8 byte);

    Stretch bg;   // Background stretch
    Stretch obj;  // Object stretch
};
