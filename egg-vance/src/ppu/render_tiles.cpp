#include "ppu.h"

#include "common/utility.h"
#include "mmu/map.h"
#include "mapentry.h"

void PPU::renderMode0()
{
    if (mmu.dispcnt.bg0) renderBackgroundMode0(0);
    if (mmu.dispcnt.bg1) renderBackgroundMode0(1);
    if (mmu.dispcnt.bg2) renderBackgroundMode0(2);
    if (mmu.dispcnt.bg3) renderBackgroundMode0(3);
}

void PPU::renderMode1()
{
    if (mmu.dispcnt.bg0) renderBackgroundMode0(0);
    if (mmu.dispcnt.bg1) renderBackgroundMode0(1);
    if (mmu.dispcnt.bg2) renderBackgroundMode2(2);
}

void PPU::renderMode2()
{
    if (mmu.dispcnt.bg2) renderBackgroundMode2(2);
    if (mmu.dispcnt.bg3) renderBackgroundMode2(3);
}

void PPU::renderBackgroundMode0(int bg)
{
    const Bgcnt& bgcnt = mmu.bgcnt[bg];
    const int scroll_x = mmu.bghofs[bg].offset;
    const int scroll_y = mmu.bgvofs[bg].offset + mmu.vcount;

    int priority = bgcnt.priority;
    int tile_x = (scroll_x / 8) % 32;
    int tile_y = (scroll_y / 8) % 32;
    int tile_size = bgcnt.palette_type ? 0x40 : 0x20;
    PixelFormat format = bgcnt.palette_type ? BPP8 : BPP4;

    int screen_x = 0;
    int x = scroll_x % 8;

    int block = initialMapBlock(bgcnt, scroll_x, scroll_y);

    while (true)
    {
        // Get base address for the used block
        u32 mapAddr = bgcnt.mapBase() + 0x800 * block;
        // Add offset for the currently used tile
        mapAddr += 2 * (32 * tile_y + tile_x);

        for (int tile = tile_x; tile < 32; ++tile)
        {
            MapEntry entry(mmu.readHalfFast(mapAddr));

            u32 addr = bgcnt.tileBase() + tile_size * entry.tile;

            // Cannot read tiles from sprite memory
            if (addr >= MAP_VRAM + 0x10000)
                return;

            for (; x < 8; ++x)
            {
                int index = readTilePixel(
                    addr, 
                    x, scroll_y % 8, 
                    entry.flip_x, 
                    entry.flip_y,
                    format
                );

                buffer_bg[bg][screen_x] = readBgColor(index, entry.palette);

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

void PPU::renderBackgroundMode2(int bg)
{
    const Bgcnt& bgcnt = mmu.bgcnt[bg];

    int pa = signExtend<int, 16>(mmu.bgpa[bg - 2]);
    int pb = signExtend<int, 16>(mmu.bgpb[bg - 2]);
    int pc = signExtend<int, 16>(mmu.bgpc[bg - 2]);
    int pd = signExtend<int, 16>(mmu.bgpd[bg - 2]);

    int line = mmu.vcount.line;

    // Inaccurate (not copying and incrementing like real GBA)
    int ref_x = signExtend<int, 28>(mmu.bgx[bg - 2]) + line * pb;
    int ref_y = signExtend<int, 28>(mmu.bgy[bg - 2]) + line * pd;

    int size = bgcnt.affineSize();

    int tiles_per_row = size / 8;

    for (int screen_x = 0; screen_x < WIDTH; ++screen_x)
    {
        int tex_x = (ref_x + screen_x * pa) >> 8;
        int tex_y = (ref_y + screen_x * pc) >> 8;

        if (tex_x < 0 || tex_x >= size || tex_y < 0 || tex_y >= size)
        {
            if (bgcnt.wraparound)
            {
                tex_x %= size;
                tex_y %= size;

                if (tex_x < 0) tex_x += size;
                if (tex_y < 0) tex_y += size;
            }
            else
            {
                buffer_bg[bg][screen_x] = COLOR_TRANSPARENT;
                continue;
            }
        }

        int tile_x = tex_x / 8;
        int tile_y = tex_y / 8;

        u32 map_addr = bgcnt.mapBase() + tile_y * tiles_per_row + tile_x;
        int tile = mmu.readByteFast(map_addr);
        u32 addr = bgcnt.tileBase() + 0x40 * tile;

        int pixel_x = tex_x % 8;
        int pixel_y = tex_y % 8;

        buffer_bg[bg][screen_x] = readBgColor(readPixel(addr, pixel_x, pixel_y, BPP8), 0);
    }
}
