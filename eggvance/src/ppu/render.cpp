#include "ppu.h"

#include "common/utility.h"
#include "oamentry.h"

void PPU::renderBgMode0(int bg)
{
    // Removes branch prediction from flipping
    static constexpr int flip_lut[2][8] = {
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 7, 6, 5, 4, 3, 2, 1, 0 }
    };

    const BackgroundControl& bgcnt = io.bgcnt[bg];

    int ref_x = io.bghofs[bg].offset;
    int ref_y = io.bgvofs[bg].offset + io.vcount;

    int tile_size = bgcnt.palette_type ? 0x40 : 0x20;
    Palette palette = bgcnt.palette_type ? P_256_1 : P_16_16;

    // Amount of blocks along axis
    int blocks_x = (bgcnt.screen_size & 0x1) ? 2 : 1;
    int blocks_y = (bgcnt.screen_size & 0x2) ? 2 : 1;

    // Wraparound
    ref_x %= (blocks_x * 256);
    ref_y %= (blocks_y * 256);

    // Current block
    int block = 0;
    switch (bgcnt.screen_size)
    {
    case 0b00: block = 0; break;
    case 0b01: block = ref_x / 256; break;
    case 0b10: block = ref_y / 256; break;
    case 0b11: block = 2 * (ref_y / 256) + ref_x / 256; break;

    default:
        EGG_UNREACHABLE;
        break;
    }

    // Tiles inside current block
    int tile_x = (ref_x / 8) % 32;
    int tile_y = (ref_y / 8) % 32;

    int pixel_x = ref_x % 8;
    int pixel_y = ref_y % 8;

    u32 base_map = 0x800 * bgcnt.map_block;
    u32 base_addr = 0x4000 * bgcnt.tile_block;

    int screen_x = 0;
    while (true)
    {
        u32 addr_map = base_map + 0x800 * block + 2 * (0x20 * tile_y + tile_x);

        // Loop over all horizontal tiles in the block
        while (tile_x++ < 32)
        {
            int entry = vram.readHalf(addr_map);
            int tile  = bits<0, 10>(entry);

            u32 addr = base_addr + tile_size * tile;
            if (addr < 0x10000)
            {
                int flip_x = bits<10, 1>(entry);
                int flip_y = bits<11, 1>(entry);
                int bank   = palette == P_256_1 ? 0 : bits<12, 4>(entry);

                for (; pixel_x < 8; ++pixel_x)
                {
                    int index = readPixel(
                        addr,
                        flip_lut[flip_x][pixel_x],
                        flip_lut[flip_y][pixel_y],
                        palette
                    );
                    backgrounds[bg][screen_x] = readBgColor(index, bank);
                    if (++screen_x == WIDTH)
                        return;
                }
            }
            else  // Prevent reading from object memory
            {
                for (; pixel_x < 8; ++pixel_x)
                {
                    backgrounds[bg][screen_x] = TRANSPARENT;
                    if (++screen_x == WIDTH)
                        return;
                }
            }
            pixel_x = 0;
            // Advance inside map
            addr_map += 2;
        }
        tile_x = 0;
        // Advance to next horizontal block
        block ^= static_cast<int>(blocks_x == 2);
    }
}

void PPU::renderBgMode2(int bg)
{
    const BackgroundControl& bgcnt = io.bgcnt[bg];

    s16 pa = io.bgpa[bg - 2].parameter;
    s16 pc = io.bgpc[bg - 2].parameter;

    int ref_x = io.bgx[bg - 2].internal;
    int ref_y = io.bgy[bg - 2].internal;

    int size = bgcnt.size();

    int tiles_per_row = size / 8;

    u32 base_map = 0x800 * bgcnt.map_block;
    u32 base_addr = 0x4000 * bgcnt.tile_block;

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
                backgrounds[bg][screen_x] = TRANSPARENT;
                continue;
            }
        }

        int tile_x = tex_x / 8;
        int tile_y = tex_y / 8;

        u32 addr_map = base_map + tile_y * tiles_per_row + tile_x;
        u32 addr = base_addr + 0x40 * vram[addr_map];

        int pixel_x = tex_x % 8;
        int pixel_y = tex_y % 8;

        backgrounds[bg][screen_x] = readBgColor(readPixel(addr, pixel_x, pixel_y, P_256_1), 0);
    }
}

void PPU::renderBgMode3(int bg)
{
    u32 addr = 2 * WIDTH * io.vcount;
    u16* pixel = vram.ptr<u16>(addr);
    std::copy_n(pixel, WIDTH, &backgrounds[bg][0]);
}

void PPU::renderBgMode4(int bg)
{
    u32 addr = (0xA000 * io.dispcnt.frame) + (WIDTH * io.vcount);
    u8* index = vram.ptr<u8>(addr);
    for (int x = 0; x < WIDTH; ++x, ++index)
    {
        backgrounds[bg][x] = readBgColor(*index, 0);
    }
}

void PPU::renderBgMode5(int bg)
{
    if (io.vcount < 128)
    {
        u32 addr = (0xA000 * io.dispcnt.frame) + (2 * 160 * io.vcount);
        u16* pixel = vram.ptr<u16>(addr);
        pixel = std::copy_n(pixel, 160, &backgrounds[bg][0]);
        std::fill_n(pixel, 80, TRANSPARENT);
    }
    else
    {
        backgrounds[bg].fill(TRANSPARENT);
    }
}

void PPU::renderObjects()
{
    int line = io.vcount;

    int mosaic_x = io.mosaic.obj.x + 1;
    int mosaic_y = io.mosaic.obj.y + 1;

    for (auto iter = oam_entries.crbegin(); iter != oam_entries.crend(); ++iter)
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

        int object_line = line - y;
        if (object_line < 0 || object_line >= rect_height)
            continue;

        int bank = oam.color_mode ? 0 : oam.palette_bank;
        int tile_size = oam.color_mode ? 0x40 : 0x20;
        Palette palette = oam.color_mode ? P_256_1 : P_16_16;

        // 1D mapping arranges tiles continuously in memory. 2D mapping arranges 
        // tiles in a 32x32 matrix. The width is halfed to 16 tiles when using 
        // 256 color mode. 
        int tiles_per_row = io.dispcnt.mapping_1d ? (width / 8) : (oam.color_mode ? 16 : 32);

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
        // color mode (and therefore tile_size). In 256/1 color mode only each 
        // second tile may be used.
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
            int tex_x = ((pa * offset_x + pb_y) >> 8) + half_width;
            int tex_y = ((pc * offset_x + pd_y) >> 8) + half_height;

            if (tex_x >= 0 && tex_x < width && tex_y >= 0 && tex_y < height)
            {
                if (flip_x) tex_x = width  - tex_x - 1;
                if (flip_y) tex_y = height - tex_y - 1;

                if (oam.mosaic)
                {
                    // Todo: Slighty different compared to real GBA
                    tex_x = mosaic_x * (tex_x / mosaic_x);
                    tex_y = mosaic_y * (tex_y / mosaic_y);
                }

                int tile_x = tex_x / 8;
                int tile_y = tex_y / 8;

                // Get tile address and account for memory mirror
                u32 addr = base_addr + tile_size * (tiles_per_row * tile_y + tile_x);
                if (addr > 0x18000)
                    addr -= 0x8000;

                int pixel_x = tex_x % 8;
                int pixel_y = tex_y % 8;

                int index = readPixel(addr, pixel_x, pixel_y, palette);
                if (index != 0)
                {
                    auto& object = objects[screen_x];

                    switch (oam.gfx_mode)
                    {
                    case GFX_NORMAL:
                    case GFX_ALPHA:
                        if (oam.priority <= object.priority)
                        {
                            object.color    = readFgColor(index, bank);
                            object.opaque   = true;
                            object.priority = oam.priority;
                            object.alpha    = oam.gfx_mode == GFX_ALPHA;
                        }
                        break;

                    case GFX_WINDOW:
                        object.window = true;
                        break;
                    }
                    objects_exist = true;
                    objects_alpha |= object.alpha;
                }
            }
        }
    }
}
