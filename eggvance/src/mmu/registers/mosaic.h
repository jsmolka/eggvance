#pragma once

#include "common/integer.h"

struct Mosaic
{
    struct Stretch
    {
        void reset();

        int x;  // Stretch along x-axis (minus 1)
        int y;  // Stretch along y-axis (minus 1)
    };

    void reset();

    void writeByte(int index, u8 byte);

    Stretch bg;   // Background stretch
    Stretch obj;  // Object stretch
};
