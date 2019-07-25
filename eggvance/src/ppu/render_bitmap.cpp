#include "ppu.h"

#include "mmu/map.h"

inline u32 frameAddr(int frame)
{
    return MAP_VRAM + 0xA000 * frame;
}

// Todo: "In BG mode 3, only one frame exists"
void PPU::renderBgMode3(int bg)
{
    u32 addr = frameAddr(mmu.dispcnt.frame);

    int y = mmu.vcount.line;
    for (int x = 0; x < WIDTH; ++x)
    {
        int offset = 2 * (WIDTH * y + x);
        int color = mmu.readHalfFast(addr + offset);

        bgs[bg][x] = color;
    }
}

void PPU::renderBgMode4(int bg)
{
    u32 addr = frameAddr(mmu.dispcnt.frame);

    int y = mmu.vcount.line;
    for (int x = 0; x < WIDTH; ++x)
    {
        int offset = WIDTH * y + x;
        int index = mmu.readByteFast(addr + offset);
        int color = readBgColor(index, 0);

        bgs[bg][x] = color;
    }
}

void PPU::renderBgMode5(int bg)
{
    u32 addr = frameAddr(mmu.dispcnt.frame);

    int y = mmu.vcount.line;
    for (int x = 0; x < WIDTH; ++x)
    {
        int color = TRANSPARENT;
        if (x < 160 && y < 128)
        {
            int offset = 2 * (160 * y + x);
            color = mmu.readHalfFast(addr + offset);
        }
        bgs[bg][x] = color;
    }
}
