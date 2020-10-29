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

    if (bgcnt[bg].mosaic && mosaic.bgs.y > 1 && !mosaic.bgs.isDominantY(vcount.value))
    {
        backgrounds[bg].flip();
    }
    else
    {
        std::invoke(render, this, bg);

        if (bgcnt[bg].mosaic && mosaic.bgs.x > 1)
        {
            for (uint x = 0; x < kScreen.x; ++x)
            {
                backgrounds[bg][x] = backgrounds[bg][mosaic.bgs.mosaicX(x)];
            }
        }
    }
}

void Gpu::renderBgMode0(uint bg)
{
    // file:///C:/Users/Julian/OneDrive/Dev/gba/GBATEK.html#lcdvramoverview

    constexpr uint kTileSize   = 8;
    constexpr uint kTileBytes  = 0x20;
    constexpr uint kBlockSize  = 256;
    constexpr uint kBlockBytes = 0x800;

    const auto& bgcnt = this->bgcnt[bg];
    const auto& size  = this->bgcnt[bg].sizeReg();

    Point origin(
        bghofs[bg].value,
        bgvofs[bg].value + vcount.value);

    origin %= size;

    Point pixel = origin % kTileSize;
    Point block = origin / kBlockSize;
    Point tile  = origin / kTileSize % (kBlockSize / kTileSize);

    for (uint x = 0; ; block.x ^= size.x / (2 * kBlockSize))
    {
        u32  map_addr = bgcnt.map_block + kBlockBytes * block.index2d(size.x / kBlockSize) + sizeof(u16) * tile.index2d(kTileBytes);  // Todo: last might be wrong name
        u16* map = reinterpret_cast<u16*>(mmu.vram.data() + map_addr);

        for (; tile.x < 32; ++tile.x, ++map)
        {
            MapEntry entry(*map);

            if (bgcnt.color_mode == kColorMode256x1)
                entry.bank = 0;

            u32 addr = bgcnt.tile_block + bgcnt.tileSize() * entry.tile;
            if (addr < 0x1'0000)
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    uint index = mmu.vram.index(addr, pixel ^ entry.flip, bgcnt.color_mode);

                    backgrounds[bg][x] = mmu.pram.colorBg(index, entry.bank);

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
        auto texture = transform(x, bg) >> 8;

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

        const auto tile  = texture / 8;
        const auto pixel = texture % 8;

        uint offset = tile.index2d(size.x / 8);
        uint entry  = mmu.vram.readFast<u8>(bgcnt.map_block + offset);
        uint index  = mmu.vram.index256x1(bgcnt.tile_block + 0x40 * entry, pixel);

        backgrounds[bg][x] = mmu.pram.colorBg(index);
    }
}

void Gpu::renderBgMode3(uint bg)
{
    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto texture = transform(x, bg) >> 8;

        if (texture >= kOrigin && texture < kScreen)
        {
            uint offset = sizeof(u16) * texture.index2d(kScreen.x);

            backgrounds[bg][x] = mmu.vram.readFast<u16>(offset) & kColorMask;
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
        const auto texture = transform(x, bg) >> 8;

        if (texture >= kOrigin && texture < kScreen)
        {
            uint offset = texture.index2d(kScreen.x);
            uint index  = mmu.vram.readFast<u8>(dispcnt.frame + offset);

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
        const auto texture = transform(x, bg) >> 8;

        if (texture >= kOrigin && texture < kBitmap)
        {
            uint offset = sizeof(u16) * texture.index2d(kBitmap.x);

            backgrounds[bg][x] = mmu.vram.readFast<u16>(dispcnt.frame + offset) & kColorMask;
        }
        else
        {
            backgrounds[bg][x] = kTransparent;
        }
    }
}

void Gpu::renderObjects()
{
    for (const auto& entry : mmu.oam.entries)
    {
        if (entry.disabled || !entry.isVisible(vcount.value))
            continue;

        const auto& origin      = entry.origin;
        const auto& center      = entry.center;
        const auto& sprite_size = entry.sprite_size;
        const auto& screen_size = entry.screen_size;
        const auto& matrix      = entry.affine ? mmu.oam.matrix(entry.matrix_idx) : kIdentity;

        uint tile_size = entry.tileSize();
        uint tiles_row = entry.tilesInRow(dispcnt.layout);
        uint bank      = entry.paletteBank();

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + vcount.value);

        uint end = std::min(origin.x + screen_size.x, kScreen.x);

        for (uint x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texture = (matrix * offset >> 8) + (sprite_size / 2);

            if (!(texture >= kOrigin && texture < sprite_size))
                continue;

            if (!entry.affine)
                texture ^= entry.flip;

            if (entry.mosaic)
            {
                texture.x = mosaic.obj.mosaicX(texture.x);
                texture.y = mosaic.obj.mosaicY(texture.y);
            }

            const auto pixel = texture % 8;
            const auto tile  = texture / 8;

            u32 addr = mmu.vram.mirror(entry.base_addr + tile_size * tile.index2d(tiles_row));
            if (addr < 0x1'4000 && dispcnt.isBitmap())
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
