#include "gpu.h"

#include "mapentry.h"
#include "matrix.h"
#include "base/macros.h"
#include "mmu/mmu.h"

Point Gpu::transform(uint x, uint bg)
{
    bg -= 2;

    return Point(
        bgpa[bg].value * static_cast<int>(x) + bgx[bg].current,
        bgpc[bg].value * static_cast<int>(x) + bgy[bg].current);
}

void Gpu::renderBg(RenderFunc render, uint bg)
{
    if ((dispcnt.layers & (1 << bg)) == 0)
        return;

    auto& bgcnt = this->bgcnt[bg];
    auto& background = backgrounds[bg];

    if (bgcnt.mosaic && mosaic.bgs.y > 1 && !mosaic.bgs.isDominantY(vcount.value))
    {
        background.flip();
    }
    else
    {
        std::invoke(render, this, bg);

        if (bgcnt.mosaic && mosaic.bgs.x > 1)
        {
            for (uint x = 0; x < kScreen.x; ++x)
            {
                background[x] = background[mosaic.bgs.mosaicX(x)];
            }
        }
    }
}

void Gpu::renderBgMode0(uint bg)
{
    switch (bgcnt[bg].color_mode)
    {
    case kColorMode16x16: renderBgMode0Impl<kColorMode16x16>(bg); break;
    case kColorMode256x1: renderBgMode0Impl<kColorMode256x1>(bg); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint ColorMode>
void Gpu::renderBgMode0Impl(uint bg)
{
    static_assert(ColorMode == kColorMode16x16 || ColorMode == kColorMode256x1);

    const auto& bgcnt = this->bgcnt[bg];
    const auto& size  = this->bgcnt[bg].sizeReg();

    Point origin(
        bghofs[bg].value,
        bgvofs[bg].value + vcount.value);

    origin %= size;

    auto pixel = origin % kTileSize;
    auto tile  = origin / kTileSize % kBlockTiles;
    auto block = origin / kBlockSize;

    for (uint x = 0; ; block.x ^= (size.x == 2 * kBlockSize) ? 1 : 0)
    {
        u32 map = bgcnt.map_block
            + kBlockBytes * block.index2d(size.x / kBlockSize)
            + kEntryBytes * tile.index2d(kBlockTiles);

        for (; tile.x < kBlockTiles; ++tile.x, map += kEntryBytes)
        {
            MapEntry entry(mmu.vram.readFast<u16>(map));

            u32 addr = bgcnt.tile_block + kTileBytes[ColorMode] * entry.tile;
            if (addr < kObjectBase)
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    uint index = ColorMode == kColorMode16x16
                        ? mmu.vram.index16x16(addr, pixel ^ entry.flip)
                        : mmu.vram.index256x1(addr, pixel ^ entry.flip);

                    backgrounds[bg][x] = ColorMode == kColorMode16x16
                        ? mmu.pram.colorBg(index, entry.bank)
                        : mmu.pram.colorBg(index);

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

void Gpu::renderBgMode2(uint bg)
{
    const auto& bgcnt = this->bgcnt[bg];
    const auto& size  = this->bgcnt[bg].sizeAff();

    for (uint x = 0; x < kScreen.x; ++x)
    {
        auto texture = transform(x, bg) >> kDecimalBits;

        if (!(texture >= kOrigin && texture < size))
        {
            if (bgcnt.wraparound)
            {
                texture.x %= size.x;
                texture.y %= size.y;

                if (texture.x < 0) texture.x += size.x;
                if (texture.y < 0) texture.y += size.y;
            }
            else
            {
                backgrounds[bg][x] = kTransparent;
                continue;
            }
        }

        const auto pixel = texture % kTileSize;
        const auto tile  = texture / kTileSize;

        uint entry = mmu.vram.readFast<u8>(bgcnt.map_block + tile.index2d(size.x / kTileSize));
        uint index = mmu.vram.index256x1(bgcnt.tile_block + kTileBytes256x1 * entry, pixel);

        backgrounds[bg][x] = mmu.pram.colorBg(index);
    }
}

void Gpu::renderBgMode3(uint bg)
{
    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> kDecimalBits;

        if (texture >= kOrigin && texture < kScreen)
        {
            uint addr = kColorBytes * texture.index2d(kScreen.x);

            backgrounds[bg][x] = mmu.vram.readFast<u16>(addr) & kColorMask;
        }
        else
        {
            backgrounds[bg][x] = kTransparent;
        }
    }
}

void Gpu::renderBgMode4(uint bg)
{
    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> kDecimalBits;

        if (texture >= kOrigin && texture < kScreen)
        {
            uint index = mmu.vram.readFast<u8>(dispcnt.frame + texture.index2d(kScreen.x));

            backgrounds[bg][x] = mmu.pram.colorBg(index);
        }
        else
        {
            backgrounds[bg][x] = kTransparent;
        }
    }
}

void Gpu::renderBgMode5(uint bg)
{
    constexpr Point kBitmap(160, 128);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> kDecimalBits;

        if (texture >= kOrigin && texture < kBitmap)
        {
            uint addr = dispcnt.frame + kColorBytes * texture.index2d(kBitmap.x);

            backgrounds[bg][x] = mmu.vram.readFast<u16>(addr) & kColorMask;
        }
        else
        {
            backgrounds[bg][x] = kTransparent;
        }
    }
}

void Gpu::renderObjects()
{
    constexpr Matrix kIdentity(1 << kDecimalBits, 0, 0, 1 << kDecimalBits);

    for (const auto& entry : mmu.oam.entries)
    {
        if (entry.disabled || !entry.isVisible(vcount.value))
            continue;

        const auto& origin      = entry.origin;
        const auto& center      = entry.center;
        const auto& sprite_size = entry.sprite_size;
        const auto& screen_size = entry.screen_size;
        const auto& matrix      = entry.affine ? mmu.oam.matrix(entry.matrix) : kIdentity;

        uint tile_bytes = entry.tileBytes();
        uint tiles_row  = entry.tilesInRow(dispcnt.layout);
        uint bank       = entry.paletteBank();

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + vcount.value);

        uint end = std::min(origin.x + screen_size.x, kScreen.x);

        for (uint x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texture = (matrix * offset >> kDecimalBits) + (sprite_size / 2);

            if (!(texture >= kOrigin && texture < sprite_size))
                continue;

            if (!entry.affine)
                texture ^= entry.flip;

            if (entry.mosaic)
            {
                texture.x = mosaic.obj.mosaicX(texture.x);
                texture.y = mosaic.obj.mosaicY(texture.y);
            }

            const auto tile  = texture / kTileSize;
            const auto pixel = texture % kTileSize;

            u32 addr = mmu.vram.mirror(entry.base_addr + tile_bytes * tile.index2d(tiles_row));
            if (addr < kObjectBaseBitmap && dispcnt.isBitmap())
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
