#include "ppu.h"

#include "mmu/map.h"
#include "obj/oamentry.h"

void PPU::renderSprites()
{
    int line = mmu.vcount;

    // 128 OAM entries
    for (int entry = 0; entry < 128; ++entry)
    {
        OamEntry oam(
            mmu.readHalfFast(MAP_OAM + 8 * entry + 0),  // Attribute 0
            mmu.readHalfFast(MAP_OAM + 8 * entry + 2),  // Attribute 1
            mmu.readHalfFast(MAP_OAM + 8 * entry + 4)   // Attribute 2
        );

        // Skip sprite if hidden
        if (!oam.attr0.rotation && oam.attr0.hide)
            continue;

        int x = oam.attr1.x;
        int y = oam.attr0.y;

        if (x >= WIDTH)  x -= 512;
        if (y >= HEIGHT) y -= 256;

        int width  = oam.spriteWidth();
        int height = oam.spriteHeight();

        // Check if the current line contains the sprite
        if (y > line || (y + height) < line)
            continue;

        int tile_offset = 0x20;
        int sprite_row = line - y;

        u32 tile_addr = MAP_VRAM + 0x10000 + tile_offset * (width / 8) * (sprite_row / 8) + 4 * (sprite_row % 8);

        for (int tile_x = 0; tile_x < (width / 8); ++tile_x)
        {
            for (int i = 0; i < 4; ++i)
            {
                int byte = mmu.readByteFast(tile_addr + i);
                int color1 = byte & 0xF;
                int color2 = byte >> 4;

                if (x >= 0 && x < WIDTH && color1 != 0)
                    draw(x, line, readSpriteColor(color1));
                x++;
                if (x >= 0 && x < WIDTH && color2 != 0)
                    draw(x, line, readSpriteColor(color2));
                x++;
            }
            tile_addr += tile_offset;
        }
    }
}
