#include "ppu.h"

#include "mmu/map.h"
#include "sprites/oamentry.h"

// Todo:
// 8-bit tiles

void PPU::updateSprites()
{
    for (int entry = 0; entry < 128; ++entry)
    {
        OamEntry oam(
            mmu.readHalfFast(MAP_OAM + 8 * entry + 0),  // Attribute 0
            mmu.readHalfFast(MAP_OAM + 8 * entry + 2),  // Attribute 1
            mmu.readHalfFast(MAP_OAM + 8 * entry + 4)   // Attribute 2
        );

        Sprite& sprite = sprites[entry];

        sprite.disabled = !oam.attr0.affine && oam.attr0.disabled;
        if (sprite.disabled)
            continue;

        sprite.x = oam.attr1.x;
        sprite.y = oam.attr0.y;

        if (sprite.x >= WIDTH)  sprite.x -= 512;
        if (sprite.y >= HEIGHT) sprite.y -= 256;

        sprite.width   = oam.width();
        sprite.height  = oam.height();
        sprite.palette = oam.attr2.palette;

        u32 tile_addr = MAP_VRAM + 0x10000 + 0x20 * oam.attr2.tile;

        int base_y = 0;
        // Iterate vertical tiles
        for (int tile_y = 0; tile_y < (sprite.height / 8); ++tile_y)
        {
            int base_x = 0;
            // Iterate horizontal tiles
            for (int tile_x = 0; tile_x < (sprite.width / 8); ++tile_x)
            {
                // Iterate vertical tile pixels
                for (int y = 0; y < 8; ++y)
                {
                    // Iterate horizontal tile pixels (2 pixels per byte)
                    for (int x = 0; x < 4; ++x)
                    {
                        int byte = mmu.readByteFast(tile_addr);

                        sprite.setPixel(
                            base_x + 2 * x,
                            base_y + y,
                            byte & 0xF,
                            oam.attr1.flip_x, 
                            oam.attr1.flip_y
                        );
                        sprite.setPixel(
                            base_x + 2 * x + 1, 
                            base_y + y, 
                            byte >> 4,
                            oam.attr1.flip_x, 
                            oam.attr1.flip_y
                        );

                        tile_addr++;
                    }
                }
                base_x += 8;

                // Tiles "overflow" if the last one is reached
                if (tile_addr > (MAP_VRAM + 0x18000))
                    tile_addr -= 0x8000;
            }
            base_y += 8;

            if (!mmu.dispcnt.sprite_1d)
                tile_addr += 0x20 * 24;
        }
    }
}

void PPU::renderSprites()
{
    int line = mmu.vcount;

    for (const Sprite& sprite : sprites)
    {
        if (sprite.disabled)
            continue;

        if (sprite.y > line || (sprite.y + sprite.height) < line)
            continue;

        for (int x = 0; x < sprite.width; ++x)
        {
            int screen_x = x + sprite.x;

            if (screen_x < 0 || screen_x >= WIDTH)
                continue;

            int color = sprite.data[line - sprite.y][x];
            if (color == 0)
                continue;

            draw(screen_x, line, readFgColor(color, sprite.palette));
        }
    }
}
