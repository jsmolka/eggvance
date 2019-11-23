#include "ppu.h"

#include "common/utility.h"
#include "common/macros.h"
#include "mmu/mmu.h"

Point PPU::transform(int bg, int x) const
{
    bg -= 2;

    return {
        (io.bgx[bg].internal + io.bgpa[bg].parameter * x) >> 8,
        (io.bgy[bg].internal + io.bgpc[bg].parameter * x) >> 8
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
        io.bghofs[bg].offset,
        io.bgvofs[bg].offset + io.vcount
    );

    origin.x %= dims.w;
    origin.y %= dims.h;

    int block = (origin / 256).offset(dims.w / 256);

    int tile_size = bgcnt.pformat ? 0x40 : 0x20;
    auto pformat = Palette::Format(bgcnt.pformat);

    auto tile  = origin / 8 % 32;
    auto pixel = origin % 8;

    int x = 0;
    while (true)
    {
        u32 addr_map = bgcnt.base_map + 0x800 * block + sizeof(u16) * tile.offset(0x20);

        while (tile.x++ < 32)
        {
            int entry = mmu.vram.readHalfFast(addr_map);
            int tile_  = bits<0, 10>(entry);

            u32 addr = bgcnt.base_tile + tile_size * tile_;
            if (addr < 0x10000)
            {
                int flip_x = bits<10, 1>(entry);
                int flip_y = bits<11, 1>(entry);
                int bank   = pformat == Palette::Format::F256 ? 0 : bits<12, 4>(entry);

                for (; pixel.x < 8; ++pixel.x)
                {
                    int index = mmu.vram.readPixel(
                        addr,
                        flip_x ? (7 - pixel.x) : pixel.x,
                        flip_y ? (7 - pixel.y) : pixel.y,
                        pformat
                    );
                    backgrounds[bg][x] = mmu.palette.colorBG(index, bank);
                    if (++x == SCREEN_W)
                        return;
                }
            }
            else  // Prevent reading from object memory
            {
                for (; pixel.x < 8; ++pixel.x)
                {
                    backgrounds[bg][x] = TRANSPARENT;
                    if (++x == SCREEN_W)
                        return;
                }
            }
            pixel.x = 0;
            // Advance inside map
            addr_map += 2;
        }
        tile.x = 0;
        // Advance to next horizontal block
        block ^= static_cast<int>((dims.w / 256) == 2);
    }
}

void PPU::renderBgMode2(int bg)
{
    const auto& bgcnt = io.bgcnt[bg];
    const auto& dims  = io.bgcnt[bg].dims_aff;

    for (int x = 0; x < SCREEN_W; ++x)
    {
        auto texture = transform(bg, x);

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

        const auto tile  = texture / TILE_SIZE;
        const auto pixel = texture % TILE_SIZE;

        int offset = tile.offset(dims.w / TILE_SIZE);
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
        const auto texture = transform(bg, x);

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
        const auto texture = transform(bg, x);

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
        const auto texture = transform(bg, x);

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
