#include "ppu.h"

#include "mmu/map.h"
#include "tiles/mapentry.h"

// Todo:
// 8bpp

// Rotate / scale: no
// Layers: 0 - 3
// Size: 256x256 - 512x512
// Tiles: 1024
// Colors: 16/16 - 256/1
// Features: scrolling, flip, mosaic, alpha blending, brightness, priority
void PPU::renderMode0()
{
    for (int priority = 3; priority > -1; --priority)
    {
        if (mmu.dispcnt.bg3 && mmu.bg3cnt.priority == priority) renderMode0Layer(3);
        if (mmu.dispcnt.bg2 && mmu.bg2cnt.priority == priority) renderMode0Layer(2);
        if (mmu.dispcnt.bg1 && mmu.bg1cnt.priority == priority) renderMode0Layer(1);
        if (mmu.dispcnt.bg0 && mmu.bg0cnt.priority == priority) renderMode0Layer(0);
    }
}

void PPU::renderMode0Layer(int layer)
{
    const Bgcnt& bgcnt = mmu.bgcnt[layer];
    const int scroll_x = mmu.bghofs[layer].offset;
    const int scroll_y = mmu.bgvofs[layer].offset + mmu.vcount;

    int tile_x = (scroll_x / 8) % 32;
    int tile_y = (scroll_y / 8) % 32;
    int tile_size = bgcnt.palette_type ? 0x40 : 0x20;

    int screen_x = 0;
    int x = scroll_x % 8;

    int block = initialMapBlock(bgcnt, scroll_x, scroll_y);

    while (true)
    {
        // Get base address for the used block
        u32 mapAddr = bgcnt.mapBase() + block * Bgcnt::map_block_size;
        // Add offset for the currently used tile
        mapAddr += 2 * (32 * tile_y + tile_x);

        for (int tile = tile_x; tile < 32; ++tile)
        {
            MapEntry entry(mmu.readHalfFast(mapAddr));

            u32 addr = bgcnt.tileBase() + tile_size * entry.tile;

            for (; x < 8; ++x)
            {
                int index = readTilePixel<BPP4>(
                    addr, 
                    x, scroll_y % 8, 
                    entry.flip_x, 
                    entry.flip_y
                );

                draw(screen_x, mmu.vcount, readBgColor(index, entry.palette));

                if (++screen_x == WIDTH)
                    return;
            }
            x = 0;
            
            mapAddr += 2;
        }
        tile_x = 0;

        block = nextHorizontalMapBlock(bgcnt, block);
    }
}

template<PPU::PixelFormat format>
int PPU::readTilePixel(u32 addr, int x, int y, bool flip_x, bool flip_y)
{
    if (flip_x) x = 7 - x;
    if (flip_y) y = 7 - y;

    if (format == BPP4)
    {
        int byte = mmu.readByteFast(
            addr + 4 * y + x / 2
        );
        return (x & 0x1) ? (byte >> 4) : (byte & 0xF);
    }
    else
    {
        return mmu.readByteFast(
            addr + 8 * y + x
        );
    }
}

int PPU::initialMapBlock(const Bgcnt& bgcnt, int offset_x, int offset_y)
{
    // Offset y contains vcount to get the correct line block

    switch (bgcnt.screen_size)
    {
    // 1x1 blocks
    case 0b00:
        return 0;

    // 2x1 blocks
    case 0b01:
        return offset_x / 256;

    // 1x2 blocks
    case 0b10:
        return (offset_y < 512) ? (offset_y / 256) : 0;

    // 2x2 blocks
    case 0b11:
        return ((offset_y < 512) ? (2 * (offset_y / 256)) : 0) + offset_x / 256;
    }
    return 0;
}

int PPU::nextHorizontalMapBlock(const Bgcnt& bgcnt, int block)
{
    // Assumes blocks starting at 0

    switch (bgcnt.screen_size)
    {
    // 1x1 blocks
    case 0b00:
        return block;

    // 2x1 blocks
    case 0b01: 
        return std::abs(block - 1);

    // 1x2 blocks
    case 0b10: 
        return block;

    // 2x2 blocks
    case 0b11:
        switch (block)
        {
        case 0: return 1;
        case 1: return 0;
        case 2: return 3;
        case 3: return 2;
        }
    }
    return 0;
}

// Layers: 0 - 2 (BG0, BG1 rendered as mode 0, BG2 rendered as mode 2)
void PPU::renderMode1()
{

}

// Rotate / scale: yes
// Layers: 2 - 3
// Size: 128x128 - 1024x1024
// Tiles: 256
// Colors: 16/16 - 256/1
// Features: mosaic, alpha blending, brightness, priority
void PPU::renderMode2()
{

}
