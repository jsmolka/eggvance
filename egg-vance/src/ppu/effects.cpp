#include "ppu.h"

void PPU::effects()
{
    mosaic();
}

void PPU::mosaic()
{
    for (int bg = 0; bg < 4; ++bg)
    {
        if (mmu.dispcnt.bg(bg) && mmu.bgcnt[bg].mosaic)
            mosaicBg(buffer[bg]);
    }
}

void PPU::mosaicBg(DoubleBuffer& buffer)
{
    int mosaic_x = mmu.mosaic.bg_x + 1;
    int mosaic_y = mmu.mosaic.bg_y + 1;

    if (mmu.vcount.line % mosaic_y == 0)
    {
        if (mosaic_x == 1)
            return;

        int color;
        for (int x = 0; x < WIDTH; ++x)
        {
            if (x % mosaic_x == 0)
                color = buffer[x];

            buffer[x] = color;
        }
    }
    else
    {
        buffer.copyPage();
    }
}