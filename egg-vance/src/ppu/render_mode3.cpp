#include "ppu.h"

#include "mmu/map.h"

void PPU::renderMode3()
{
    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < 240; ++x)
    {
        // Each pixel is stored as a 16-bit color
        int offset = 240 * 2 * y + 2 * x;
        int color = mmu.readHalf(frame_addr + offset);

        pixel(x, y, color);
    }
}