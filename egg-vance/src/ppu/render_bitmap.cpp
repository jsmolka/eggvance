#include "ppu.h"

#include "mmu/memory_map.h"

void PPU::renderBitmap()
{
    int y = mmu.vcount;

    u32 frame_addr = mmu.dispcnt.frameAddr();

    for (int x = 0; x < 240; ++x)
    {
        int value = mmu.readByte(frame_addr + (y * 240 + x));
        int color = mmu.readHalf(MAP_PALETTE + (2 * value));

        pixel(x, y, color);
    }
}