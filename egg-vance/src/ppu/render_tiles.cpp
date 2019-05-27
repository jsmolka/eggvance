#include "ppu.h"

#include "mmu/map.h"
#include "tiles/mapentry.h"

// Todo:
// 8bpp

void PPU::drawTileMap(int x, int y, int map_block)
{
    u32 mapAddr = MAP_VRAM + Bgcnt::map_block_size * map_block;

    for (int tile_y = 0; tile_y < 20; ++tile_y)
    {
        for (int tile_x = 0; tile_x < 30; ++tile_x)
        {
            MapEntry entry(mmu.readHalfFast(mapAddr));
        
            u32 addr = mmu.bg0cnt.tileAddr() + 0x20 * entry.tile;
        }
    }
}

void PPU::renderText()
{
    const Bgcnt& bgcnt = mmu.bg0cnt;

    u32 mapAddr = 0x0600FE1C;  // bgcnt.mapAddr();

    MapEntry entry(mmu.readHalfFast(mapAddr));

    u32 addr = bgcnt.tileAddr() + 0x20  * entry.tile;

    // Iterate vertical tile pixels
    for (int y = 0; y < 8; ++y)
    {
        // Iterate horizontal tile pixels (2 pixels per byte)
        for (int x = 0; x < 4; x++)
        {
            int byte = mmu.readByteFast(addr);

            draw(2 * x, y, readBgColor(byte & 0xF, entry.palette));
            draw(2 * x + 1, y, readBgColor(byte >> 4, entry.palette));

            addr++;
        }
    }
}

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
    const int scroll_x = mmu.bgvofs[layer].offset;
    const int scroll_y = mmu.bghofs[layer].offset;

    //if (scroll_y < mmu.vcount)
    //    return;

    u32 mapAddr = bgcnt.mapAddr();

    int base_y = 0;
    for (int tile_y = 0; tile_y < 32; ++tile_y)
    {
        int base_x = 0;
        for (int tile_x = 0; tile_x < 32; ++tile_x)
        {
            MapEntry entry(mmu.readHalfFast(mapAddr));

            u32 addr = bgcnt.tileAddr() + 0x20 * entry.tile;

            // Iterate vertical tile pixels
            for (int y = 0; y < 8; ++y)
            {
                // Iterate horizontal tile pixels (2 pixels per byte)
                for (int x = 0; x < 4; x++)
                {
                    if ((base_x + 2 * x + 1) < WIDTH && (base_y + y) < HEIGHT)
                    {
                        int byte = mmu.readByteFast(addr);

                        draw(base_x + 2 * x, base_y + y, readBgColor(byte & 0xF, entry.palette));
                        draw(base_x + 2 * x + 1, base_y + y, readBgColor(byte >> 4, entry.palette));
                    }

                    addr++;
                }
            }
            base_x += 8;

            mapAddr += 2;
        }
        base_y += 8;
    }
}

// Layers: 0 - 2 (0, 1 rendered as mode 0, 2 rendered as mode 2)
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
