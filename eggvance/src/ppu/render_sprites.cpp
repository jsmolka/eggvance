#include "ppu.h"

#include "mmu/map.h"
#include "oamentry.h"

// Todo: maybe move mosaic out of this function

void PPU::renderSprites()
{
    if (!mmu.dispcnt.sprites)
        return;

    int line = mmu.vcount.line;

    int mosaic_x = mmu.mosaic.sprite_x + 1;
    int mosaic_y = mmu.mosaic.sprite_y + 1;

    for (int entry = 127; entry > -1; --entry)
    {
        OAMEntry oam(
            mmu.readHalfFast(MAP_OAM + 8 * entry + 0),  // Attribute 0
            mmu.readHalfFast(MAP_OAM + 8 * entry + 2),  // Attribute 1
            mmu.readHalfFast(MAP_OAM + 8 * entry + 4)   // Attribute 2
        );

        if (!oam.affine && oam.disabled)
            continue;

        int x = oam.x;
        int y = oam.y;

        // Wraparound
        if (x >= WIDTH)  x -= 512;
        if (y >= HEIGHT) y -= 256;

        int width  = oam.width();
        int height = oam.height();

        // Bounding rectangle (invisible outside)
        int rect_width  = width;
        int rect_height = height;

        if (oam.double_size)
        {
            rect_width  *= 2;
            rect_height *= 2;
        }

        // Check if the sprite is visible
        int sprite_line = line - y;
        if (sprite_line < 0 || sprite_line >= rect_height)
            continue;

        PixelFormat format = oam.color_mode ? BPP8 : BPP4;
        int tile_size = oam.color_mode ? 0x40 : 0x20;
        // 2D mapping arranges sprites in a 32x32 tile matrix
        int tile_size_row = tile_size * (mmu.dispcnt.mapping_1d ? (width / 8) : 32);

        bool flip_x = !oam.affine && oam.flip_x;
        bool flip_y = !oam.affine && oam.flip_y;

        // Initialize with identity
        s16 pa = 0x100;
        s16 pb = 0x000;
        s16 pc = 0x000;
        s16 pd = 0x100;

        if (oam.affine)
        {
            pa = mmu.readHalfFast(MAP_OAM + 0x20 * oam.paramter + 0x06);  
            pb = mmu.readHalfFast(MAP_OAM + 0x20 * oam.paramter + 0x0E);  
            pc = mmu.readHalfFast(MAP_OAM + 0x20 * oam.paramter + 0x16);  
            pd = mmu.readHalfFast(MAP_OAM + 0x20 * oam.paramter + 0x1E);  
        }

        // Rotation center
        int center_x = x + rect_width / 2;
        int center_y = y + rect_height / 2;

        // Offset from rotation center
        int rect_x = -rect_width / 2;
        int rect_y = line - center_y;

        u32 base_addr = MAP_VRAM + 0x10000 + tile_size * oam.tile;

        for (; rect_x < rect_width / 2; ++rect_x)
        {
            // Texture coordinates inside the sprite
            int tex_x = ((pa * rect_x + pb * rect_y) >> 8) + width / 2;
            int tex_y = ((pc * rect_x + pd * rect_y) >> 8) + height / 2;

            if (tex_x >= 0 && tex_x < width && tex_y >= 0 && tex_y < height)
            {
                if (flip_x) tex_x = width  - 1 - tex_x;
                if (flip_y) tex_y = height - 1 - tex_y;

                if (oam.mosaic)
                {
                    // Slighty different to real GBA
                    tex_x = mosaic_x * (tex_x / mosaic_x);
                    tex_y = mosaic_y * (tex_y / mosaic_y);
                }

                int tile_x = tex_x / 8;
                int tile_y = tex_y / 8;

                // Get tile address and account for overflow
                u32 addr = base_addr + tile_size_row * tile_y + tile_size * tile_x;
                if (addr > MAP_VRAM + 0x18000)
                    addr -= 0x8000;

                int pixel_x = tex_x % 8;
                int pixel_y = tex_y % 8;

                int index = readPixel(addr, pixel_x, pixel_y, format);
                if (index != 0)
                {
                    int color;
                    if (format == BPP4)
                        color = readFgColor(index, oam.palette_bank);
                    else
                        color = readFgColor(index, 0);
                    
                    int screen_x = center_x + rect_x;
                    if (screen_x >= 0 && screen_x < WIDTH)
                    {
                        sprites[screen_x].pixel = color;
                        sprites[screen_x].entry = entry;
                        sprites[screen_x].semi_transparent = oam.gfx_mode == 1;
                        sprites[screen_x].priority = oam.priority;
                    }
                }
            }
        }
    }
}