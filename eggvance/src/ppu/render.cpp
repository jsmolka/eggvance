#include "ppu.h"

#include <shell/macros.h>
#include <shell/operators.h>

#include "mapentry.h"
#include "matrix.h"

Point Ppu::transform(uint x, uint bg)
{
    return Point(
        bgpa[bg - 2] * static_cast<int>(x) + bgx[bg - 2],
        bgpc[bg - 2] * static_cast<int>(x) + bgy[bg - 2]);
}

void Ppu::renderBg(RenderFunc render, uint bg)
{
    if ((dispcnt.layers & (1 << bg)) == 0)
        return;

    auto& bgcnt = this->bgcnt[bg];
    auto& background = backgrounds[bg];

    if (bgcnt.mosaic && mosaic.bgs.y > 1 && !mosaic.bgs.isDominantY(vcount))
    {
        background.flip();
    }
    else
    {
        std::invoke(render, this, bg);

        if (bgcnt.mosaic && mosaic.bgs.x > 1)
        {
            for (auto [x, color] : shell::enumerate(background))
            {
                color = background[mosaic.bgs.mosaicX(x)];
            }
        }
    }
}

void Ppu::renderBgMode0(uint bg)
{
    switch (ColorMode(bgcnt[bg].color_mode))
    {
    case ColorMode::C16x16: renderBgMode0Impl<ColorMode::C16x16>(bg); break;
    case ColorMode::C256x1: renderBgMode0Impl<ColorMode::C256x1>(bg); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<ColorMode kColorMode>
void Ppu::renderBgMode0Impl(uint bg)
{
    const auto& bgcnt = this->bgcnt[bg];
    const auto& size  = this->bgcnt[bg].sizeReg();

    Point origin(bghofs[bg], bgvofs[bg] + vcount);

    origin %= size;

    auto pixel = origin % kTileSize;
    auto tile  = origin / kTileSize % kMapBlockTiles;
    auto block = origin / kMapBlockSize;

    for (uint x = 0; ; block.x ^= size.x > kMapBlockSize)
    {
        u32 map = bgcnt.map_block
            + kMapBlockBytes * block.index2d(size.x / kMapBlockSize)
            + kMapEntryBytes * tile.index2d(kMapBlockTiles);

        for (; tile.x < kMapBlockTiles; ++tile.x, map += kMapEntryBytes)
        {
            MapEntry entry(vram.readFast<u16>(map));

            u32 addr = bgcnt.tile_block + kTileBytes[uint(kColorMode)] * entry.tile;
            if (addr < kObjectBase)
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    uint index = kColorMode == ColorMode::C16x16
                        ? vram.index16x16(addr, pixel ^ entry.flip)
                        : vram.index256x1(addr, pixel ^ entry.flip);

                    backgrounds[bg][x] = kColorMode == ColorMode::C16x16
                        ? pram.colorBg(index, entry.bank)
                        : pram.colorBg(index);

                    if (++x == kScreen.x)
                        return;
                }
            }
            else
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    backgrounds[bg][x] = kTransparent;

                    if (++x == kScreen.x)
                        return;
                }
            }
            pixel.x = 0;
        }
        tile.x = 0;
    }
}

void Ppu::renderBgMode2(uint bg)
{
    const auto& bgcnt = this->bgcnt[bg];
    const auto& size  = this->bgcnt[bg].sizeAff();

    for (auto [x, color] : shell::enumerate(backgrounds[bg]))
    {
        auto texel = transform(x, bg) >> kDecimalBits;

        if (!(texel >= kOrigin && texel < size))
        {
            if (bgcnt.wraparound)
            {
                texel %= size;

                if (texel.x < 0) texel.x += size.x;
                if (texel.y < 0) texel.y += size.y;
            }
            else
            {
                color = kTransparent;
                continue;
            }
        }

        const auto pixel = texel % kTileSize;
        const auto tile  = texel / kTileSize;

        uint entry = vram.readFast<u8>(bgcnt.map_block + tile.index2d(size.x / kTileSize));
        uint index = vram.index256x1(bgcnt.tile_block + kTileBytes[uint(ColorMode::C256x1)] * entry, pixel);

        color = pram.colorBg(index);
    }
}

void Ppu::renderBgMode3(uint bg)
{
    for (auto [x, color] : shell::enumerate(backgrounds[bg]))
    {
        const auto texel = transform(x, bg) >> kDecimalBits;

        if (texel >= kOrigin && texel < kScreen)
        {
            u32 addr = kColorBytes * texel.index2d(kScreen.x);

            color = vram.readFast<u16>(addr) & kColorMask;
        }
        else
        {
            color = kTransparent;
        }
    }
}

void Ppu::renderBgMode4(uint bg)
{
    for (auto [x, color] : shell::enumerate(backgrounds[bg]))
    {
        const auto texel = transform(x, bg) >> kDecimalBits;

        if (texel >= kOrigin && texel < kScreen)
        {
            uint index = vram.readFast<u8>(dispcnt.frame + texel.index2d(kScreen.x));

            color = pram.colorBg(index);
        }
        else
        {
            color = kTransparent;
        }
    }
}

void Ppu::renderBgMode5(uint bg)
{
    constexpr Point kBitmap(160, 128);

    for (auto [x, color] : shell::enumerate(backgrounds[bg]))
    {
        const auto texel = transform(x, bg) >> kDecimalBits;

        if (texel >= kOrigin && texel < kBitmap)
        {
            u32 addr = dispcnt.frame + kColorBytes * texel.index2d(kBitmap.x);

            color = vram.readFast<u16>(addr) & kColorMask;
        }
        else
        {
            color = kTransparent;
        }
    }
}

void Ppu::renderObjects()
{
    constexpr Matrix kIdentity = { 1 << kDecimalBits, 0, 0, 1 << kDecimalBits };

    s64 cycles = dispcnt.oam_free ? 954 : 1210;

    for (const auto& entry : oam.entries)
    {
        if (entry.disabled || !entry.isVisible(vcount))
            continue;

        const auto& origin      = entry.origin;
        const auto& center      = entry.center;
        const auto& sprite_size = entry.sprite_size;
        const auto& screen_size = entry.screen_size;
        const auto& matrix      = entry.affine ? oam.matrices[entry.matrix] : kIdentity;

        uint tile_bytes = entry.tileBytes();
        uint tiles_row  = entry.tilesPerRow(dispcnt.layout);
        uint bank       = entry.paletteBank();

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + vcount);

        uint end = std::min(origin.x + screen_size.x, kScreen.x);

        for (uint x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texel = (matrix * offset >> kDecimalBits) + (sprite_size / 2);

            if (!(texel >= kOrigin && texel < sprite_size))
                continue;

            if (!entry.affine)
                texel ^= entry.flip;

            if (entry.mosaic)
            {
                texel.x = mosaic.obj.mosaicX(texel.x);
                texel.y = mosaic.obj.mosaicY(texel.y);
            }

            const auto tile  = texel / kTileSize;
            const auto pixel = texel % kTileSize;

            u32 addr = vram.mirror(entry.base_addr + tile_bytes * tile.index2d(tiles_row));
            if (addr < kObjectBaseBitmap && dispcnt.isBitmap())
                continue;

            auto& object = objects[x];

            uint index = vram.index(addr, pixel, entry.color_mode);
            if ( index != 0)
            {
                switch (ObjectMode(entry.object_mode))
                {
                case ObjectMode::Normal:
                case ObjectMode::Alpha:
                    if (entry.priority < object.priority || !object.opaque())
                    {
                        object.color    = pram.colorFgOpaque(index, bank);
                        object.priority = entry.priority;
                        object.alpha    = entry.object_mode == ObjectMode::Alpha;
                    }
                    break;

                case ObjectMode::Window:
                    object.window = true;
                    break;

                case ObjectMode::Invalid:
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

        cycles -= entry.cycles();
        if (cycles <= 0)
            break;
    }
}
