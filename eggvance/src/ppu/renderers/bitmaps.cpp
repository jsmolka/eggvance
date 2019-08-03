#include "ppu/ppu.h"

void PPU::renderBgMode3(int bg)
{
    u32 addr = 2 * WIDTH * mmu.vcount;
    u16* pixel = mmu.vram.ptr<u16>(addr);
    std::copy_n(pixel, WIDTH, &bgs[bg][0]);
}

void PPU::renderBgMode4(int bg)
{
    u32 addr = (0xA000 * mmu.dispcnt.frame) + (WIDTH * mmu.vcount);
    u8* index = mmu.vram.ptr<u8>(addr);
    for (int x = 0; x < WIDTH; ++x, ++index)
        bgs[bg][x] = readBgColor(*index, 0);
}

void PPU::renderBgMode5(int bg)
{
    if (mmu.vcount < 128)
    {
        u32 addr = (0xA000 * mmu.dispcnt.frame) + (2 * 160 * mmu.vcount);
        u16* pixel = mmu.vram.ptr<u16>(addr);
        std::copy_n(pixel, 160, &bgs[bg][0]);
        std::fill_n(&bgs[bg][160], 80, TRANSPARENT);
    }
    else
    {
        bgs[bg].fill(TRANSPARENT);
    }
}
