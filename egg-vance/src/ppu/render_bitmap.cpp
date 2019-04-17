#include "ppu.h"

#include "common/memory_map.h"

void PPU::renderBitmap()
{
    u32 base = 0x6000000;
    u32 pale = 0x5000000;

    //mmu->dump(base, 0x9FFF);

    for (int x = 0; x < 240; ++x)
    {
        for (int y = 0; y < 160; ++y)
        {
            int value = mmu->readByte(base + (160 * y + x));
            int color = mmu->readHalf(pale + (2 * value));

            drawPixel(x, y, color);
        }
    }
}