#include "ppu.h"

#include "mmu/map.h"

void PPU::renderBitmapMode3()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        // Each pixel is stored as a 16-bit color
        int offset = WIDTH * 2 * y + 2 * x;
        int color = mmu.readHalfFast(frame_addr + offset);

        draw(x, y, color);
    }
}

void PPU::renderBitmapMode4()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        // Each pixel is stored as a 8-bit palette indices
        int offset = WIDTH * y + x;
        int index = mmu.readByteFast(frame_addr + offset);
        // Palette stores 16-bit colors
        int color = readBgColor(index, 0);

        draw(x, y, color);
    }
}

void PPU::renderBitmapMode5()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    // Like mode 3, but with dimensions 160x128
    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        int color;
        if (x < 160 && y < 128)
        {
            // Each pixel is stored as a 16-bit color
            int offset = 160 * 2 * y + 2 * x;
            color = mmu.readHalfFast(frame_addr + offset);
        }
        else
        {
            // Color 0 seems to be used outside the bitmap
            color = readBgColor(0, 0);
        }
        draw(x, y, color);
    }
}
