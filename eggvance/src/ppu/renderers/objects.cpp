#include "ppu/ppu.h"

#include "ppu/enums.h"
#include "ppu/oamentry.h"

void PPU::renderObjects()
{
    int line = mmu.vcount;

    int mosaic_x = mmu.mosaic.obj.x + 1;
    int mosaic_y = mmu.mosaic.obj.y + 1;

    for (auto iter = mmu.oam_entries.crbegin(); iter != mmu.oam_entries.crend(); ++iter)
    {
        const OAMEntry& oam = *iter;
        if (!oam.affine && oam.disabled)
            continue;

        int x = oam.x;
        int y = oam.y;

        // Wraparound
        if (x >= WIDTH)  x -= 512;
        if (y >= HEIGHT) y -= 256;

        int width  = oam.width();
        int height = oam.height();

        // Bounding rectangle dimensions
        int rect_width  = width;
        int rect_height = height;

        if (oam.double_size)
        {
            rect_width  <<= 1;
            rect_height <<= 1;
        }

        int sprite_line = line - y;
        if (sprite_line < 0 || sprite_line >= rect_height)
            continue;

        int palette = oam.color_mode ? 0 : oam.palette;
        int tile_size = oam.color_mode ? 0x40 : 0x20;
        PixelFormat format = oam.color_mode ? BPP8 : BPP4;

        // 1D mapping arranges tiles continuously in memory. 2D mapping arranges 
        // tiles in a 32x32 matrix. The width is halfed to 16 tiles when using 
        // 256 color mode. 
        int tiles_per_row = mmu.dispcnt.mapping_1d ? (width / 8) : (oam.color_mode ? 16 : 32);

        bool flip_x = !oam.affine && oam.flip_x;
        bool flip_y = !oam.affine && oam.flip_y;

        // Initalize with identity
        s16 pa = 0x100;
        s16 pb = 0x000;
        s16 pc = 0x000;
        s16 pd = 0x100;

        if (oam.affine)
        {
            pa = *pas[oam.paramter];
            pb = *pbs[oam.paramter];
            pc = *pcs[oam.paramter];
            pd = *pds[oam.paramter];
        }

        // Rotation center
        int center_x = x + rect_width / 2;
        int center_y = y + rect_height / 2;

        // Rotation center offset
        int offset_x = -rect_width / 2;
        int offset_y = line - center_y;

        // The base tile defines the start of the object independent of the
        // color mode (and therefore tile_size). In 256 bit color mode only 
        // each second tile may be used.
        u32 base_addr = 0x10000 + 0x20 * oam.tile;

        int half_width  = width / 2;
        int half_height = height / 2;

        int pb_y = pb * offset_y;
        int pd_y = pd * offset_y;

        // Do not calculate data outside of the screen
        int screen_x = center_x + offset_x;
        if (screen_x < 0)
        {
            offset_x -= screen_x;
            screen_x = 0;
        }

        for (; offset_x < rect_width / 2; ++offset_x, ++screen_x)
        {
            if (screen_x >= WIDTH)
                break;

            // Texture coordinates inside the sprite
            int texture_x = ((pa * offset_x + pb_y) >> 8) + half_width;
            int texture_y = ((pc * offset_x + pd_y) >> 8) + half_height;

            if (texture_x >= 0 && texture_x < width && texture_y >= 0 && texture_y < height)
            {
                if (flip_x) texture_x = width  - texture_x - 1;
                if (flip_y) texture_y = height - texture_y - 1;

                if (oam.mosaic)
                {
                    // Todo: Slighty different compared to real GBA
                    texture_x = mosaic_x * (texture_x / mosaic_x);
                    texture_y = mosaic_y * (texture_y / mosaic_y);
                }

                int tile_x = texture_x / 8;
                int tile_y = texture_y / 8;

                // Get tile address and account for memory mirror
                u32 addr = base_addr + tile_size * (tiles_per_row * tile_y + tile_x);
                if (addr > 0x18000)
                    addr -= 0x8000;

                int pixel_x = texture_x % 8;
                int pixel_y = texture_y % 8;

                int index = readPixel(addr, pixel_x, pixel_y, format);
                if (index != 0)
                {
                    switch (oam.gfx_mode)
                    {
                    case GFX_NORMAL:
                    case GFX_ALPHA:
                        if (oam.priority <= obj[screen_x].priority)
                        {
                            obj[screen_x].color = readFgColor(index, palette);
                            obj[screen_x].priority = oam.priority;
                            obj[screen_x].mode = oam.gfx_mode;
                            obj_exist = true;
                        }
                        break;

                    case GFX_WINDOW:
                        obj[screen_x].window = true;
                        break;
                    }
                }
            }
        }
    }
}
