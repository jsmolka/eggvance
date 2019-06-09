#include "ppu.h"

#include "mmu/map.h"
#include "utility.h"

void PPU::renderMode3()
{
    renderBitmap(3);
}

void PPU::renderMode4()
{
    renderBitmap(4);
}

void PPU::renderMode5()
{
    renderBitmap(5);
}

void PPU::renderBitmap(int mode)
{
    if (!mmu.dispcnt.bg2)
        return;

    int y = mmu.vcount.line;

    int mosaic_x = 1;
    int mosaic_y = 1;

    if (mmu.bg2cnt.mosaic)
    {
        mosaic_x = mmu.mosaic.bg_x + 1;
        mosaic_y = mmu.mosaic.bg_y + 1;

        y = getMosaic(y, mosaic_y);
    }

    u32 addr = mmu.dispcnt.frameAddr();

    switch (mode)
    {
    case 3:
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            int offset = 2 * (WIDTH * y + getMosaic(x, mosaic_x));
            int color = mmu.readHalfFast(addr + offset);

            buffer_bg2[x] = color;
        }
        break;
    }

    case 4:
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            int offset = WIDTH * y + getMosaic(x, mosaic_x);
            int index = mmu.readByteFast(addr + offset);
            int color = readBgColor(index, 0);

            buffer_bg2[x] = color;
        }
        break;
    }

    case 5:
    {
        for (int x = 0; x < 160; ++x)
        {
            int color = COLOR_TRANSPARENT;
            if (x < 160 && y < 128)
            {
                int offset = 2 * (160 * y + getMosaic(x, mosaic_x));
                color = mmu.readHalfFast(addr + offset);
            }
            buffer_bg2[x] = color;
        }
        break;
    }
    }
}
