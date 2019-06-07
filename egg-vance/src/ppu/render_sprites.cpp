#include "ppu.h"

#include "mmu/map.h"
#include "sprites/oamentry.h"

void PPU::renderSprites()
{
    if (!mmu.dispcnt.sprites)
        return;

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

        // Wraparound
        if (x >= WIDTH)  x -= 512;
        if (y >= HEIGHT) y -= 256;

        // Sprite size
        int width = oam.width();
        int height = oam.height();

        // Affine rectangle size
        int rect_width = width;
        int rect_height = height;

        if (oam.attr0.double_size)
        {
            rect_width *= 2;
            rect_height *= 2;
        }

        if (y > line || (y + rect_height) < line)
            continue;

        PixelFormat format = oam.attr0.color_mode ? BPP8 : BPP4;
        int tile_size = oam.attr0.color_mode ? 0x40 : 0x20;
        // Todo: is this correct?
        // There are 3 other sprites in the same row when using 2D mapping
        int tile_row_size = tile_size * (mmu.dispcnt.sprite_1d ? (width / 8) : (width / 2));

        bool flip_x = !oam.attr0.affine && oam.attr1.flip_x;
        bool flip_y = !oam.attr0.affine && oam.attr1.flip_y;

        // Identity matrix by default
        s16 pa = 0x100;
        s16 pb = 0x000;
        s16 pc = 0x000;
        s16 pd = 0x100;

        if (oam.attr0.affine)
        {
            pa = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x06);  
            pb = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x0E);  
            pc = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x16);  
            pd = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x1E);  
        }

        u32 base_addr = MAP_VRAM + 0x10000 + oam.attr2.tile * tile_size;

        // Rotation center
        int center_x = x + rect_width / 2;
        int center_y = y + rect_height / 2;

        // Offset from rotation center
        int rect_x = -rect_width / 2;
        int rect_y = line - center_y;

        for (; rect_x < rect_width / 2; ++rect_x)
        {
            // Texture coordinates inside sprite
            int tex_x = ((pa * rect_x + pb * rect_y) >> 8) + width / 2;
            int tex_y = ((pc * rect_x + pd * rect_y) >> 8) + height / 2;

            if (tex_x >= 0 && tex_x < width && tex_y >= 0 && tex_y < height)
            {
                if (flip_x) tex_x = width  - 1 - tex_x;
                if (flip_y) tex_y = height - 1 - tex_y;

                int tile_x = tex_x / 8;
                int tile_y = tex_y / 8;
                int pixel_x = tex_x % 8;
                int pixel_y = tex_y % 8;

                u32 addr = base_addr + tile_row_size * tile_y + tile_size * tile_x;

                // Tile overflow
                if (addr > (MAP_VRAM + 0x18000))
                    addr -= 0x8000;

                int color = readPixel(addr, pixel_x, pixel_y, format);
                if (color != 0)
                {
                    if (format == BPP4)
                        color = readFgColor(color, oam.attr2.palette);
                    else
                        color = readFgColor(color, 0);
                    
                    int screen_x = center_x + rect_x;
                    if (screen_x >= 0 && screen_x < WIDTH)
                        buffer_sprites[oam.attr2.priority][screen_x] = color;
                }
            }
        }
    }
}
