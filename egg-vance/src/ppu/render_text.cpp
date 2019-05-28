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
    const int scroll_y = mmu.bgvofs[layer].offset;

    int tile_x = scroll_x / 8;
    int tile_y = (scroll_y + mmu.vcount) / 8;
    int tile_size = bgcnt.palette_type ? 0x40 : 0x20;

    tile_x %= 32;
    tile_y %= 32;

    // Actual x on the screen
    int screen_x = 0;
    // Start x for the first tile
    int x = scroll_x % 8;
    
    for (int block = scroll_x / 256; block < bgcnt.width() / 256; ++block)
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
                    x, (scroll_y + mmu.vcount) % 8, 
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
