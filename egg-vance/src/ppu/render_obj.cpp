#include "ppu.h"

#include "mmu/map.h"
#include "obj/oamentry.h"

void PPU::renderSprites()
{
    int y = mmu.vcount;

    // 128 OAM entries
    for (int oam = 0; oam < 128; ++oam)
    {
        OamEntry entry(
            mmu.readHalfFast(MAP_OAM + 8 * oam + 0),  // Attribute 0
            mmu.readHalfFast(MAP_OAM + 8 * oam + 2),  // Attribute 1
            mmu.readHalfFast(MAP_OAM + 8 * oam + 4)   // Attribute 2
        );

        // Skip if sprite is hidding
        if (!entry.attr0.rotation && entry.attr0.hide)
            continue;

        int sx = entry.attr1.x;
        int sy = entry.attr0.y;

        int width  = entry.spriteWidth();
        int height = entry.spriteHeight();

        // Todo: probably false
        // Apply double size flag if needed
        if (entry.attr0.rotation && entry.attr0.double_size)
        {
            width  *= 2;
            height *= 2;

            // Todo: adjust sy for large tiles
            // if (width == 128)
        }

         // Check if sprite is in the current scanline
        if (sy > y || (sy + height) < y)
            continue;

        // Base address of the sprite
        u32 tile_addr = MAP_VRAM + 0x10000 + 0x20 * entry.attr2.tile_number;

        for (int yy = 0; yy < (height / 8); ++yy)
        {
            for (int xx = 0; xx < (width / 8); ++xx)
            {
                for (int ty = 0; ty < 8; ++ty)
                {
                    for (int tx = 0; tx < 4; ++tx)
                    {
                        int byte = mmu.readByteFast(tile_addr);
                        int color_left = byte & 0xF; 
                        int color_right = byte >> 4;

                        draw(sx + 8 * xx + 2 * tx, sy + 8 * yy + ty, readSpriteColor(color_left));
                        draw(sx + 8 * xx + 2 * tx + 1, sy + 8 * yy + ty, readSpriteColor(color_right));

                        tile_addr++;
                    }
                }
            }
        }
    }
}
