#pragma once

#include "oam.h"
#include "palette.h"
#include "vram.h"

class PPU
{
public:
    void reset();

    OAM oam;
    Palette palette;
    VRAM vram;

    struct IO
    {
        struct DPC
        {
            int mode;
        } dispcnt;
    } io;
};

extern PPU ppu;
