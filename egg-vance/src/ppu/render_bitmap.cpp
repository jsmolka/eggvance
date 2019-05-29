#include "ppu.h"

#include "mmu/map.h"

// Rotate / scale: yes
// Layers: 2
// Size: 240x160
// Tiles: 1
// Colors: 32768
// Features: mosaic, alpha blending, brightness, priority
void PPU::renderMode3()
{
    if (!mmu.dispcnt.bg2)
        return;

    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        int offset = 2 * (WIDTH * y + x);
        int color = mmu.readHalfFast(frame_addr + offset);

        draw(x, y, color);
    }
}

// Rotate / scale: yes
// Layers: 2
// Size: 240x160
// Tiles: 2
// Colors: 256/1
// Features: mosaic, alpha blending, brightness, priority
void PPU::renderMode4()
{
    if (!mmu.dispcnt.bg2)
        return;

    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        int offset = WIDTH * y + x;
        int index = mmu.readByteFast(frame_addr + offset);
        int color = readBgColor(index, 0);

        draw(x, y, color);
    }
}

// Rotate / scale: yes
// Layers: 2
// Size: 160x128
// Tiles: 2
// Colors: 32768
// Features: mosaic, alpha blending, brightness, priority
void PPU::renderMode5()
{
    if (!mmu.dispcnt.bg2)
        return;

    u32 frame_addr = mmu.dispcnt.frameAddr();

    int y = mmu.vcount;
    for (int x = 0; x < WIDTH; ++x)
    {
        // Todo: default in other emus is pinkish color
        // Use transparent color by default
        int color = 0x8000;

        if (x < 160 && y < 128)
        {
            int offset = 2 * (160 * y + x);
            color = mmu.readHalfFast(frame_addr + offset);
        }
        draw(x, y, color);
    }
}
