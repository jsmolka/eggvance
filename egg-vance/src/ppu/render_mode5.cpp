#include "ppu.h"

#include "mmu/memory_map.h"

void PPU::renderMode5()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    // Like mode 3, but with dimensions 160x128
    int y = mmu.vcount;
    for (int x = 0; x < 240; ++x)
    {
        int color;
        if (x < 160 && y < 128)
        {
            // Each pixel is stored as a 16-bit color
            int offset = 160 * 2 * y + 2 * x;
            color = mmu.readHalf(frame_addr + offset);
        }
        else
        {
            // Color 0 seems to be used outside the bitmap
            color = mmu.readHalf(MAP_PALETTE);
        }
        pixel(x, y, color);
    }
}