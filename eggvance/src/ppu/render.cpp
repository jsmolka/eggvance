#include "ppu.h"

#include "common/utility.h"
#include "common/macros.h"
#include "mmu/mmu.h"
#include "mapentry.h"

Point PPU::transformBG(int x, int bg) const
{
    bg -= 2;

    return {
        (io.bgx[bg].current + io.bgpa[bg].value * x) >> 8,
        (io.bgy[bg].current + io.bgpc[bg].value * x) >> 8
    };
}

void PPU::renderBg(RenderFunc func, int bg)
{
    if (~io.dispcnt.layers & (1 << bg))
        return;

    if (mosaicAffected(bg))
    {
        if (mosaicDominant())
        {
            (this->*func)(bg);
            mosaic(bg);
        }
        else
        {
            backgrounds[bg].flip();
        }
    }
    else
    {
        (this->*func)(bg);
    }
}

void PPU::renderBgMode0(int bg)
{
    const auto& bgcnt = io.bgcnt[bg];
    const auto& dims  = io.bgcnt[bg].dims_reg;

    Point origin(
        io.bghofs[bg].value,
        io.bgvofs[bg].value + io.vcount
    );

    origin.x %= dims.w;
    origin.y %= dims.h;

    auto pixel = origin % 8;
    auto block = origin / 256;
    auto tile  = origin / 8 % 32;

    for (int x = 0; x < SCREEN_W; block.x ^= (dims.w / 256) == 2)
    {
        int offset = 0x800 * block.offset(dims.w / 256) + 2 * tile.offset(0x20);

        u16* map = mmu.vram.data<u16>(bgcnt.base_map + offset);

        for (; tile.x < 32 && x < SCREEN_W; ++tile.x, ++map)
        {
            MapEntry entry(*map);

            if (bgcnt.pal_format == Palette::Format::F256)
            {
                entry.bank = 0;
            }

            u32 addr = bgcnt.base_tile + (0x20 << bgcnt.pal_format) * entry.tile;
            if (addr < 0x1'0000)
            {
                for (; pixel.x < 8 && x < SCREEN_W; ++pixel.x, ++x)
                {
                    int index = mmu.vram.readPixel(
                        addr,
                        pixel.x ^ (0x7 * entry.flip_x),
                        pixel.y ^ (0x7 * entry.flip_y),
                        Palette::Format(bgcnt.pal_format)
                    );
                    backgrounds[bg][x] = mmu.palette.colorBG(index, entry.bank);
                }
            }
            else
            {
                for (; pixel.x < 8 && x < SCREEN_W; ++pixel.x, ++x)
                {
                    backgrounds[bg][x] = TRANSPARENT;
                }
            }
            pixel.x = 0;
        }
        tile.x = 0;
    }
}

void PPU::renderBgMode2(int bg)
{
    const auto& bgcnt = io.bgcnt[bg];
    const auto& dims  = io.bgcnt[bg].dims_aff;

    for (int x = 0; x < SCREEN_W; ++x)
    {
        auto texture = transformBG(x, bg);

        if (!dims.contains(texture))
        {
            if (bgcnt.wraparound)
            {
                texture.x %= dims.w;
                texture.y %= dims.h;

                if (texture.x < 0) texture.x += dims.w;
                if (texture.y < 0) texture.y += dims.h;
            }
            else
            {
                backgrounds[bg][x] = TRANSPARENT;
                continue;
            }
        }

        const auto tile  = texture / 8;
        const auto pixel = texture % 8;

        int offset = tile.offset(dims.w / 8);
        int entry  = mmu.vram.readByteFast(bgcnt.base_map + offset);
        int index  = mmu.vram.readIndexByte(bgcnt.base_tile + 0x40 * entry, pixel);

        backgrounds[bg][x] = mmu.palette.colorBG(index);
    }
}

void PPU::renderBgMode3(int bg)
{
    constexpr Dimensions dims(SCREEN_W, SCREEN_H);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transformBG(x, bg);

        if (!dims.contains(texture))
        {
            backgrounds[bg][x] = TRANSPARENT;
            continue;
        }

        int offset = sizeof(u16) * texture.offset(dims.w);

        backgrounds[bg][x] = mmu.vram.readHalfFast(offset) & COLOR_MASK;
    }
}

void PPU::renderBgMode4(int bg)
{
    constexpr Dimensions dims(SCREEN_W, SCREEN_H);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transformBG(x, bg);

        if (!dims.contains(texture))
        {
            backgrounds[bg][x] = TRANSPARENT;
            continue;
        }

        int offset = texture.offset(dims.w);
        int index  = mmu.vram.readByteFast(io.dispcnt.base_frame + offset);

        backgrounds[bg][x] = mmu.palette.colorBG(index);
    }
}

void PPU::renderBgMode5(int bg)
{
    constexpr Dimensions dims(160, 128);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transformBG(x, bg);

        if (!dims.contains(texture))
        {
            backgrounds[bg][x] = TRANSPARENT;
            continue;
        }

        int offset = sizeof(u16) * texture.offset(dims.w);

        backgrounds[bg][x] = mmu.vram.readHalfFast(io.dispcnt.base_frame + offset) & COLOR_MASK;
    }
}

void PPU::renderObjects()
{
    int line = io.vcount;

    for (int e = 127; e >= 0; --e)
    {
        const auto& entry = mmu.oam.entry(e);
        if ((!entry.affine && entry.disabled) || entry.isUninitialized())
            continue;

        int x = entry.x;
        int y = entry.y;

        // Wraparound
        if (x >= SCREEN_W) x -= 512;
        if (y >= SCREEN_H) y -= 256;

        int width  = entry.width();
        int height = entry.height();

        // Bounding rectangle dimensions
        int rect_width  = width;
        int rect_height = height;

        if (entry.double_size)
        {
            rect_width  <<= 1;
            rect_height <<= 1;
        }

        int sprite_line = line - y;
        if (sprite_line < 0 || sprite_line >= rect_height)
            continue;

        int bank = entry.color_mode ? 0 : entry.palette_bank;
        int tile_size = entry.color_mode ? 0x40 : 0x20;
        Palette::Format pformat = entry.color_mode ? Palette::Format::F256 : Palette::Format::F16;

        // 1D mapping arranges tiles continuously in memory. 2D mapping arranges 
        // tiles in a 32x32 matrix. The width is halfed to 16 tiles when using 
        // 256 color mode. 
        int tiles_per_row = io.dispcnt.mapping_1d ? (width / 8) : (entry.color_mode ? 16 : 32);

        bool flip_x = !entry.affine && entry.flip_x;
        bool flip_y = !entry.affine && entry.flip_y;

        // Initalize with identity
        s16 pa = 0x100;
        s16 pb = 0x000;
        s16 pc = 0x000;
        s16 pd = 0x100;

        if (entry.affine)
        {
            pa = mmu.oam.pa(entry.parameter);
            pb = mmu.oam.pb(entry.parameter);
            pc = mmu.oam.pc(entry.parameter);
            pd = mmu.oam.pd(entry.parameter);

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
        u32 base_addr = 0x10000 + 0x20 * entry.tile;

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
            if (screen_x >= SCREEN_W)
                break;

            // Texture coordinates inside the sprite
            int tex_x = ((pa * offset_x + pb_y) >> 8) + half_width;
            int tex_y = ((pc * offset_x + pd_y) >> 8) + half_height;

            if (tex_x >= 0 && tex_x < width && tex_y >= 0 && tex_y < height)
            {
                if (flip_x) tex_x = width  - tex_x - 1;
                if (flip_y) tex_y = height - tex_y - 1;

                if (entry.mosaic)
                {
                    // Todo: Slighty different compared to real GBA
                    tex_x = io.mosaic.obj.sourceX(tex_x);
                    tex_y = io.mosaic.obj.sourceY(tex_y);
                }

                int tile_x = tex_x / 8;
                int tile_y = tex_y / 8;

                // Get tile address and account for memory mirror
                u32 addr = base_addr + tile_size * (tiles_per_row * tile_y + tile_x);
                if (addr >= 0x18000)
                    addr -= 0x08000;

                int pixel_x = tex_x % 8;
                int pixel_y = tex_y % 8;

                int index = mmu.vram.readPixel(addr, pixel_x, pixel_y, pformat);
                if (index != 0)
                {
                    auto& object = objects[screen_x];

                    switch (entry.gfx_mode)
                    {
                    case GFX_NORMAL:
                    case GFX_ALPHA:
                        if (entry.priority <= object.prio)
                        {
                            object.color  = mmu.palette.colorFGOpaque(index, bank);
                            object.opaque = true;
                            object.prio   = entry.priority;
                            object.alpha  = entry.gfx_mode == GFX_ALPHA;
                        }
                        break;

                    case GFX_WINDOW:
                        object.window = true;
                        break;

                    default:
                        EGG_UNREACHABLE;
                        break;
                    }
                    objects_exist = true;
                    objects_alpha |= object.alpha;
                }
            }
        }
    }
}
