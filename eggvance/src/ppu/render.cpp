#include "ppu.h"

#include "matrix.h"
#include "mapentry.h"
#include "common/macros.h"
#include "mmu/mmu.h"

Point PPU::transform(int x, int bg)
{
    bg -= 2;

    return Point(
        io.bgx[bg] + io.bgpa[bg] * x,
        io.bgy[bg] + io.bgpc[bg] * x
    );
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
        io.bghofs[bg],
        io.bgvofs[bg] + io.vcount
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

            if (bgcnt.color_mode == int(ColorMode::C256x1))
            {
                entry.bank = 0;
            }

            u32 addr = bgcnt.base_tile + (0x20 << bgcnt.color_mode) * entry.tile;
            if (addr < 0x1'0000)
            {
                for (; pixel.x < 8 && x < SCREEN_W; ++pixel.x, ++x)
                {
                    int index = mmu.vram.index(
                        addr,
                        { pixel.x ^ (0x7 * entry.flip_x), pixel.y ^ (0x7 * entry.flip_y), },
                        ColorMode(bgcnt.color_mode)
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
        auto texture = transform(x, bg) >> 8;

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
        int index  = mmu.vram.index256x1(bgcnt.base_tile + 0x40 * entry, pixel);

        backgrounds[bg][x] = mmu.palette.colorBG(index);
    }
}

void PPU::renderBgMode3(int bg)
{
    static constexpr Dimensions dims(SCREEN_W, SCREEN_H);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

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
    static constexpr Dimensions dims(SCREEN_W, SCREEN_H);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

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
    static constexpr Dimensions dims(160, 128);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

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
    for (const auto& entry : mmu.oam.entries)
    {
        if (entry.isDisabled() || !entry.isVisible(io.vcount))
            continue;

        const auto& origin = entry.origin;
        const auto& center = entry.center;
        const auto& dims   = entry.dims;
        const auto& bounds = entry.bounds;
        const auto& matrix = entry.affine 
            ? mmu.oam.matrix(entry.matrix)
            : identity_matrix;

        int size  = entry.tileSize();
        int bank  = entry.paletteBank();
        int tiles = entry.tilesPerRow(ObjectMapping(io.dispcnt.obj_mapping));

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + io.vcount
        );

        int end = std::min(origin.x + bounds.w, SCREEN_W);

        for (int x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texture = (matrix.multiply(offset) >> 8) + (dims / 2);

            if (!dims.contains(texture))
                continue;

            if (entry.flipX()) texture.x ^= entry.dims.w - 1;
            if (entry.flipY()) texture.y ^= entry.dims.h - 1;
            if (entry.mosaic)
            {
                texture.x = io.mosaic.obj.mosaicX(texture.x);
                texture.y = io.mosaic.obj.mosaicY(texture.y);
            }

            const auto tile  = texture / 8;
            const auto pixel = texture % 8;

            u32 addr = mmu.vram.mirror(entry.base_tile + size * tile.offset(tiles));
            if (addr < 0x1'4000 && io.dispcnt.isBitmap())
                continue;

            int index = mmu.vram.index(addr, pixel, ColorMode(entry.color_mode));
            if (index == 0)
                continue;

            auto& object = objects[x];

            switch (ObjectMode(entry.mode))
            {
            case ObjectMode::Alpha:
            case ObjectMode::Normal:
                if (entry.prio < object.prio)
                {
                    object.color = mmu.palette.colorFGOpaque(index, bank);
                    object.prio  = entry.prio;
                    object.alpha = entry.mode == int(ObjectMode::Alpha);
                }
                break;

            case ObjectMode::Window:
                object.window = true;
                break;

            case ObjectMode::Invalid:
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
