#include "ppu.h"

#include "mmu/map.h"

void PPU::renderMode4()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        // Each pixel is stored as a 8-bit palette indices
        int offset = WIDTH * y + x;
        int index = mmu.readByteFast(frame_addr + offset);
        // Palette stores 16-bit colors
        int color = mmu.readHalfFast(MAP_PALETTE + 2 * index);

        draw(x, y, color);
    }
}