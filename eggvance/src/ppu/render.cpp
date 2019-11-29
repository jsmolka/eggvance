#include "ppu.h"

#include "common/utility.h"
#include "common/macros.h"
#include "mmu/mmu.h"
#include "matrix.h"
#include "mapentry.h"

Point PPU::transformBG(int x, int bg) const
{
    bg -= 2;

    return Point(
        io.bgx[bg].current + io.bgpa[bg].value * x,
        io.bgy[bg].current + io.bgpc[bg].value * x
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
        auto texture = transformBG(x, bg) >> 8;

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
    constexpr Dimensions dims(SCREEN_W, SCREEN_H);

    for (int x = 0; x < SCREEN_W; ++x)
    {
        const auto texture = transformBG(x, bg) >> 8;

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
        const auto texture = transformBG(x, bg) >> 8;

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
        const auto texture = transformBG(x, bg) >> 8;

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
    const auto& entries = mmu.oam.entries;

    for (auto entry = entries.crbegin(); entry != entries.crend(); ++entry)
    {
        if (entry->isDisabled())
            continue;

        const auto& origin = entry->origin;
        const auto& dims   = entry->dims;
        const auto  bounds = entry->dims * (1 << entry->double_size);

        if ((origin.x + bounds.w) < 0 || origin.x >= SCREEN_W)
            continue;

        int line = io.vcount - origin.y;
        if (line < 0 || line >= bounds.h)
            continue;

        int size  = entry->tileSize();
        int bank  = entry->paletteBank();
        int tiles = entry->tilesPerRow(io.dispcnt.obj_mapping);

        const Matrix matrix = entry->affine
            ? mmu.oam.matrix(entry->matrix_index)
            : identity_matrix;

        const Point center(
            origin.x + bounds.w / 2,
            origin.y + bounds.h / 2
        );

        Point offset(
            origin.x - center.x - std::min(origin.x, 0),
            io.vcount - center.y
        );

        int end = std::min(origin.x + bounds.w, SCREEN_W);

        for (int x = center.x + offset.x; x < end; ++offset.x, ++x)
        {
            auto texture = matrix.multiply(offset) >> 8;

            texture.x += dims.w / 2;
            texture.y += dims.h / 2;

            if (!dims.contains(texture))
                continue;

            if (entry->flipX()) texture.x ^= entry->dims.w - 1;
            if (entry->flipY()) texture.y ^= entry->dims.h - 1;
            if (entry->mosaic)
            {
                texture.x = io.mosaic.obj.mosaicX(texture.x);
                texture.y = io.mosaic.obj.mosaicY(texture.y);
            }

            const auto tile  = texture / 8;
            const auto pixel = texture % 8;

            u32 addr = mmu.vram.mirror(entry->base_tile + size * tile.offset(tiles));
            if (addr < 0x1'4000 && io.dispcnt.isBitmap())
                continue;

            int index = mmu.vram.index(addr, pixel, ColorMode(entry->color_mode));
            if (index == 0)
                continue;

            auto& object = objects[x];

            switch (GraphicsMode(entry->graphics_mode))
            {
            case GraphicsMode::Alpha:
            case GraphicsMode::Normal:
                if (entry->prio <= object.prio)
                {
                    object.color  = mmu.palette.colorFGOpaque(index, bank);
                    object.opaque = true;
                    object.prio   = entry->prio;
                    object.alpha  = entry->graphics_mode == int(GraphicsMode::Alpha);
                }
                break;

            case GraphicsMode::Window:
                object.window = true;
                break;

            case GraphicsMode::Invalid:
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
