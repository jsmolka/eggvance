#include "ppu.h"

#include "mmu/map.h"
#include "sprites/oamentry.h"

void PPU::renderSprites()
{
    int line = mmu.vcount.line;

    for (int entry = 127; entry > -1; --entry)
    {
        OamEntry oam(
            mmu.readHalfFast(MAP_OAM + 8 * entry + 0),  // Attribute 0
            mmu.readHalfFast(MAP_OAM + 8 * entry + 2),  // Attribute 1
            mmu.readHalfFast(MAP_OAM + 8 * entry + 4)   // Attribute 2
        );

        if (!oam.attr0.affine && oam.attr0.disabled)
            continue;

        int x = oam.attr1.x;
        int y = oam.attr0.y;

        if (x >= WIDTH)  x -= 512;
        if (y >= HEIGHT) y -= 256;

        int width = oam.width();
        int height = oam.height();

        if (y > line || (y + height) < line)
            continue;

        PixelFormat format = oam.attr0.color_mode ? BPP8 : BPP4;
        int tile_size = oam.attr0.color_mode ? 0x40 : 0x20;

        bool flip_x = !oam.attr0.affine && oam.attr1.flip_x;
        bool flip_y = !oam.attr0.affine && oam.attr1.flip_y;

        // Current line in the sprite
        int sprite_y = flip_y ? (height - 1 - (line - y)) : (line - y);
        // Current vertical tile
        int tile_y = sprite_y / 8;
        // Current vertical pixel in the tile
        int pixel_y = sprite_y % 8;
        
        u32 addr = MAP_VRAM + 0x10000 + oam.attr2.tile * tile_size;
        if (mmu.dispcnt.sprite_1d)
            addr += tile_size * 8 * tile_y;
        else
            addr += tile_size * 32 * tile_y;

        // Screen coordinates
        int screen_x = flip_x ? (x + width - 1) : x;
        int screen_y = line;

        int offset = flip_x ? -1 : 1;
        for (int tile_x = 0; tile_x < (width / 8); ++tile_x)
        {
            for (int pixel_x = 0; pixel_x < 8; ++pixel_x)
            {
                if (screen_x >= 0 && screen_x < WIDTH)
                {
                    int color = readPixel(addr, pixel_x, pixel_y, format);
                    if (color != 0)
                    {
                        if (format == BPP4)
                            color = readFgColor(color, oam.attr2.palette);

                        draw(screen_x, screen_y, color);
                    }
                }
                screen_x += offset;
            }
            addr += tile_size;

            // Tile overflow
            if (addr > (MAP_VRAM + 0x18000))
                addr -= 0x8000;
        }
    }
}
