#include "ppu.h"

#include "mmu/map.h"
#include "sprites/oamentry.h"

// Todo: handle tile "overflow" 18000h -> 10000h
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

        sprite.width  = oam.spriteWidth();
        sprite.height = oam.spriteHeight();
        sprite.palette = oam.attr2.palette;

        int tile_y_offset = 0x20 * (mmu.dispcnt.sprite_1d ? (sprite.width / 8) : 32);

        u32 base_tile = MAP_VRAM + 0x10000 + 0x20 * oam.attr2.tile;

        // Current sprite coordinates
        int sprite_y = 0;

        // Iterate vertical tiles
        for (int tile_y = 0; tile_y < (sprite.height / 8); ++tile_y)
        {
            int sprite_x = 0;

            // Offset in current tile
            int offset = 0;

            // Iterate horizontal tiles
            for (int tile_x = 0; tile_x < (sprite.width / 8); ++tile_x)
            {
                // Iterate vertical tile pixels
                for (int y = 0; y < 8; ++y)
                {
                    // Iterate horizontal tile pixels (2 pixel per byte)
                    for (int x = 0; x < 4; ++x)
                    {
                        int byte = mmu.readByteFast(base_tile + offset);
                        int value1 = byte & 0xF;
                        int value2 = byte >> 4;

                        sprite.setPixel(sprite_x + 2 * x, sprite_y + y, value1, oam.attr1.flip_x, oam.attr1.flip_y);
                        sprite.setPixel(sprite_x + 2 * x + 1, sprite_y + y, value2, oam.attr1.flip_x, oam.attr1.flip_y);

                        offset++;
                    }
                }
                sprite_x += 8;
            }
            sprite_y += 8;

            base_tile += tile_y_offset;
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

            draw(screen_x, line, readSpriteColor(color, sprite.palette));
        }
    }
}
