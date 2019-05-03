#include "ppu.h"

#include "mmu/map.h"

void PPU::renderMode3()
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