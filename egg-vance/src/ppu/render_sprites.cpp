#include "ppu.h"

#include "mmu/map.h"
#include "sprites/oamentry.h"

#include "common/format.h"
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

        // Initialize matrix with identity
        s16 pa = 0x100;  // dx
        s16 pb = 0x000;  // dmx
        s16 pc = 0x000;  // dy
        s16 pd = 0x100;  // dmy

        if (oam.attr0.affine)
        {
            pa = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x06);  
            pb = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x0E);  
            pc = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x16);  
            pd = mmu.readHalfFast(MAP_OAM + 0x20 * oam.attr1.paramter + 0x1E);  
        }

        int rect_width = width;
        int rect_height = height;
        int center_x = x + width / 2;
        int center_y = y + height / 2;

        u32 baseAddr = MAP_VRAM + 0x10000 + oam.attr2.tile * tile_size;

        // Screen coordinates
        int screen_x = x;
        int screen_y = line;

        int rect_y = line - center_y;

        for (int rect_x = -rect_width / 2; rect_x < rect_width / 2; ++rect_x)
        {
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

                u32 addr = baseAddr + tile_size * tile_x;
                if (mmu.dispcnt.sprite_1d)
                    addr += tile_size * 8 * tile_y;
                else
                    addr += tile_size * 32 * tile_y;

                // Tile overflow
                if (addr > (MAP_VRAM + 0x18000))
                    addr -= 0x8000;

                int color = readPixel(addr, pixel_x, pixel_y, format);
                if (color != 0)
                {
                    if (format == BPP4)
                        color = readFgColor(color, oam.attr2.palette);

                    if (screen_x >= 0 && screen_x < WIDTH)
                        draw(screen_x, screen_y, color);
                }
            }
            screen_x++;
        }
    }
}
