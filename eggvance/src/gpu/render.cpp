#include "gpu.h"

#include "base/macros.h"
#include "gpu/matrix.h"
#include "gpu/mapentry.h"
#include "mmu/mmu.h"

Point Gpu::transform(int x, int bg)
{
    bg -= 2;

    return Point(
        io.bgx[bg].current + io.bgpa[bg].value * x,
        io.bgy[bg].current + io.bgpc[bg].value * x
    );
}

void Gpu::renderBg(RenderFunc func, int bg)
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

void Gpu::renderBgMode0(int bg)
{
    const auto& bgcnt = io.bgcnt[bg];
    const auto& dims  = io.bgcnt[bg].dimsReg();

    Point origin(
        io.bghofs[bg].value,
        io.bgvofs[bg].value + io.vcount.value
    );

    origin.x %= dims.x;
    origin.y %= dims.y;

    auto pixel = origin % 8;
    auto block = origin / 256;
    auto tile  = origin / 8 % 32;

    for (int x = 0; x < kScreen.x; block.x ^= (dims.x / 256) == 2)
    {
        int offset = 0x800 * block.index2d(dims.x / 256) + 2 * tile.index2d(0x20);

        u16* map = mmu.vram.data<u16>(bgcnt.map_block + offset);

        for (; tile.x < 32 && x < kScreen.x; ++tile.x, ++map)
        {
            MapEntry entry(*map);

            if (bgcnt.color_mode == kColorMode256x1)
                entry.bank = 0;

            u32 addr = bgcnt.tile_block + (0x20 << bgcnt.color_mode) * entry.tile;
            if (addr < 0x1'0000)
            {
                for (; pixel.x < 8 && x < kScreen.x; ++pixel.x, ++x)
                {
                    int index = mmu.vram.index(
                        addr,
                        Point(pixel.x ^ (0x7 * entry.flip_x), pixel.y ^ (0x7 * entry.flip_y)),
                        ColorMode(bgcnt.color_mode)
                    );
                    backgrounds[bg][x] = mmu.pram.colorBg(index, entry.bank);
                }
            }
            else
            {
                for (; pixel.x < 8 && x < kScreen.x; ++pixel.x, ++x)
                {
                    backgrounds[bg][x] = kTransparent;
                }
            }
            pixel.x = 0;
        }
        tile.x = 0;
    }
}

void Gpu::renderBgMode2(int bg)
{
    const auto& bgcnt = io.bgcnt[bg];
    const auto& dims  = io.bgcnt[bg].dimsAff();

    for (int x = 0; x < kScreen.x; ++x)
    {
        auto texture = transform(x, bg) >> 8;

        if (!(texture >= kOrigin && texture < dims))
        {
            if (bgcnt.wraparound)
            {
                texture.x %= dims.x;
                texture.y %= dims.y;

                if (texture.x < 0) texture.x += dims.x;
                if (texture.y < 0) texture.y += dims.y;
            }
            else
            {
                backgrounds[bg][x] = kTransparent;
                continue;
            }
        }

        const auto tile  = texture / 8;
        const auto pixel = texture % 8;

        int offset = tile.index2d(dims.x / 8);
        int entry  = mmu.vram.readFast<u8>(bgcnt.map_block + offset);
        int index  = mmu.vram.index256x1(bgcnt.tile_block + 0x40 * entry, pixel);

        backgrounds[bg][x] = mmu.pram.colorBg(index);
    }
}

void Gpu::renderBgMode3(int bg)
{
    for (int x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

        if (!(texture >= kOrigin && texture < kScreen))
        {
            backgrounds[bg][x] = kTransparent;
            continue;
        }

        int offset = sizeof(u16) * texture.index2d(kScreen.x);

        backgrounds[bg][x] = mmu.vram.readFast<u16>(offset) & kColorMask;
    }
}

void Gpu::renderBgMode4(int bg)
{
    for (int x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

        if (!(texture >= kOrigin && texture < kScreen))
        {
            backgrounds[bg][x] = kTransparent;
            continue;
        }

        int offset = texture.index2d(kScreen.x);
        int index  = mmu.vram.readFast<u8>(io.dispcnt.frame + offset);

        backgrounds[bg][x] = mmu.pram.colorBg(index);
    }
}

void Gpu::renderBgMode5(int bg)
{
    constexpr Point kBitmap(160, 128);

    for (int x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

        if (!(texture >= kOrigin && texture < kBitmap))
        {
            backgrounds[bg][x] = kTransparent;
            continue;
        }

        int offset = sizeof(u16) * texture.index2d(kBitmap.x);

        backgrounds[bg][x] = mmu.vram.readFast<u16>(io.dispcnt.frame + offset) & kColorMask;
    }
}

void Gpu::renderObjects()
{
    for (const auto& entry : mmu.oam.entries)
    {
        if (entry.disabled || !entry.isVisible(io.vcount.value))
            continue;

        const auto origin      = entry.origin;
        const auto center      = entry.center;
        const auto sprite_size = entry.sprite_size;
        const auto screen_size = entry.screen_size;
        const auto matrix      = entry.affine ? mmu.oam.matrix(entry.matrix_idx) : kIdentityMatrix;

        uint tile_size = entry.tileSize();
        uint tiles_row = entry.tilesInRow(io.dispcnt.layout);
        uint bank      = entry.paletteBank();

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + io.vcount.value);

        int end = std::min(origin.x + screen_size.x, kScreen.x);

        for (int x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texture = (matrix * offset >> 8) + (sprite_size / 2);

            if (!(texture >= kOrigin && texture < sprite_size))
                continue;

            if (!entry.affine)
            {
                texture.x ^= entry.flip_x;
                texture.y ^= entry.flip_y;
            }

            if (entry.mosaic)
            {
                texture.x = io.mosaic.obj.mosaicX(texture.x);
                texture.y = io.mosaic.obj.mosaicY(texture.y);
            }

            const auto tile  = texture / 8;
            const auto pixel = texture % 8;

            u32 addr = mmu.vram.mirror(entry.base_addr + tile_size * tile.index2d(tiles_row));
            if (addr < 0x1'4000 && io.dispcnt.isBitmap())
                continue;

            auto& object = objects[x];

            uint index = mmu.vram.index(addr, pixel, entry.color_mode);
            if ( index != 0)
            {
                switch (entry.object_mode)
                {
                case kObjectModeNormal:
                case kObjectModeAlpha:
                    if (entry.priority < object.priority || !object.opaque())
                    {
                        object.color    = mmu.pram.colorFgOpaque(index, bank);
                        object.priority = entry.priority;
                        object.alpha    = entry.object_mode == kObjectModeAlpha;
                    }
                    break;

                case kObjectModeWindow:
                    object.window = true;
                    break;

                case kObjectModeInvalid:
                    break;

                default:
                    SHELL_UNREACHABLE;
                    break;
                }
                objects_exist = true;
                objects_alpha |= object.alpha;
            }

            object.priority = std::min(object.priority, entry.priority);
        }
    }
}
