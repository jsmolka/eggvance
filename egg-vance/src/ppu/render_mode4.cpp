#include "ppu.h"

#include "mmu/map.h"

void PPU::renderMode4()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < 240; ++x)
    {
        // Each pixel is stored as a 8-bit palette indices
        int offset = 240 * y + x;
        int index = mmu.readByte(frame_addr + offset);
        // Palette stores 16-bit colors
        int color = mmu.readHalf(MAP_PALETTE + 2 * index);

        pixel(x, y, color);
    }
}