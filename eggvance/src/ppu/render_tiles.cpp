#include "ppu.h"

#include "common/utility.h"
#include "mapentry.h"

void PPU::renderBgMode0(int bg)
{
    const BackgroundControl& bgcnt = mmu.bgcnt[bg];

    int tile_size = bgcnt.palette_type ? 0x40 : 0x20;
    PixelFormat format = bgcnt.palette_type ? BPP8 : BPP4;

    int ref_x = mmu.bghofs[bg].offset;
    int ref_y = mmu.bgvofs[bg].offset + mmu.vcount;

    // Amount of blocks along axis
    int blocks_x = (bgcnt.screen_size & 0x1) ? 2 : 1;
    int blocks_y = (bgcnt.screen_size & 0x2) ? 2 : 1;

    // Wraparound
    ref_x %= (blocks_x * 256);
    ref_y %= (blocks_y * 256);

    // Initial block
    int block = 0;
    switch (bgcnt.screen_size)
    {
    case 0b00: block = 0; break;
    case 0b01: block = ref_x / 256; break;
    case 0b10: block = ref_y / 256; break;
    case 0b11: block = 2 * (ref_y / 256) + ref_x / 256; break;
    }

    // Tiles inside initial block
    int tile_x = (ref_x / 8) % 32;
    int tile_y = (ref_y / 8) % 32;

    int pixel_x = ref_x % 8;
    int pixel_y = ref_y % 8;

    int screen_x = 0;
    while (true)
    {
        u32 map_addr = bgcnt.mapBase() + 0x800 * block + 2 * (0x20 * tile_y + tile_x);

        for (int tile = tile_x; tile < 32; ++tile)
        {
            MapEntry entry(mmu.vram.get<u16>(map_addr));

            u32 addr = bgcnt.tileBase() + tile_size * entry.tile;

            for (; pixel_x < 8; ++pixel_x)
            {
                int color = TRANSPARENT;
                // Prevent reading sprite memory
                if (addr < MAP_VRAM + 0x10000)
                {
                    int index = readPixel(
                        addr,
                        entry.flip_x ? (7 - pixel_x) : pixel_x,
                        entry.flip_y ? (7 - pixel_y) : pixel_y,
                        format
                    );

                    if (format == BPP4)
                        color = readBgColor(index, entry.palette);
                    else
                        color = readBgColor(index, 0);
                        
                }
                bgs[bg][screen_x] = color;

                if (++screen_x == WIDTH)
                    return;
            }
            pixel_x = 0;
            
            // Next map entry
            map_addr += 2;
        }
        tile_x = 0;

        // Next vertical block
        if (blocks_x == 2)
        {
            if (block % 2 == 1)
                block--;
            else
                block++;
        }
    }
}

void PPU::renderBgMode2(int bg)
{
    const BackgroundControl& bgcnt = mmu.bgcnt[bg];

    s16 pa = mmu.bgpa[bg - 2].value;
    s16 pc = mmu.bgpc[bg - 2].value;

    int line = mmu.vcount;

    int ref_x = mmu.bgx[bg - 2].internal;
    int ref_y = mmu.bgy[bg - 2].internal;

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
                bgs[bg][screen_x] = TRANSPARENT;
                continue;
            }
        }

        int tile_x = tex_x / 8;
        int tile_y = tex_y / 8;

        u32 map_addr = bgcnt.mapBase() + tile_y * tiles_per_row + tile_x;
        int tile = mmu.vram[map_addr];
        u32 addr = bgcnt.tileBase() + 0x40 * tile;

        int pixel_x = tex_x % 8;
        int pixel_y = tex_y % 8;

        bgs[bg][screen_x] = readBgColor(readPixel(addr, pixel_x, pixel_y, BPP8), 0);
    }
}
