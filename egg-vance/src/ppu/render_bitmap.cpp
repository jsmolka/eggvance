#include "ppu.h"

#include "common/memory_map.h"

void PPU::renderBitmap()
{
    int line = mmu.vcount.ly;

    u32 base = 0x6000000;
    u32 pale = 0x5000000;

    for (int x = 0; x < 240; ++x)
    {
        int value = mmu.readByte(base + (240 * line + x));
        int color = mmu.readHalf(pale + (2 * value));

        drawPixel(x, line, color);
    }
}