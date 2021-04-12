#include "ppu.h"

#include <shell/macros.h>
#include <shell/operators.h>

#include "mapentry.h"
#include "matrix.h"
#include "frontend/videocontext.h"

void Ppu::render()
{
    if (dispcnt.blank)
    {
        auto& scanline = video_ctx.scanline(vcount);
        scanline.fill(0xFFFF'FFFF);
        return;
    }

    if (!dispcnt.isActive())
        return;

    for (auto& background : backgrounds)
        background.buffer.flip();

    if (objects_exist)
    {
        objects.fill(ObjectLayer());
        objects_exist = false;
        objects_alpha = false;
    }

    if (dispcnt.enabled & Layer::Flag::Obj)
    {
        renderObjects();
    }

    switch (dispcnt.mode)
    {
    case 0:
        renderBackground(&Ppu::renderBackground0, backgrounds[0]);
        renderBackground(&Ppu::renderBackground0, backgrounds[1]);
        renderBackground(&Ppu::renderBackground0, backgrounds[2]);
        renderBackground(&Ppu::renderBackground0, backgrounds[3]);
        compose(uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2 | Layer::Flag::Bg3));
        break;

    case 1:
        renderBackground(&Ppu::renderBackground0, backgrounds[0]);
        renderBackground(&Ppu::renderBackground0, backgrounds[1]);
        renderBackground(&Ppu::renderBackground2, backgrounds[2]);
        compose(uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2));
        break;

    case 2:
        renderBackground(&Ppu::renderBackground2, backgrounds[2]);
        renderBackground(&Ppu::renderBackground2, backgrounds[3]);
        compose(uint(Layer::Flag::Bg2 | Layer::Flag::Bg3));
        break;

    case 3:
        renderBackground(&Ppu::renderBackground3, backgrounds[2]);
        compose(uint(Layer::Flag::Bg2));
        break;

    case 4:
        renderBackground(&Ppu::renderBackground4, backgrounds[2]);
        compose(uint(Layer::Flag::Bg2));
        break;

    case 5:
        renderBackground(&Ppu::renderBackground5, backgrounds[2]);
        compose(uint(Layer::Flag::Bg2));
        break;
    }
}

void Ppu::renderBackground(BackgroundRender render, Background& background)
{
    if ((dispcnt.enabled & (1 << background.id)) == 0)
        return;

    if (background.control.mosaic && mosaic.bgs.isMosaicY() && !mosaic.bgs.isDominantY(vcount))
    {
        background.buffer.flip();
    }
    else
    {
        std::invoke(render, this, background);

        if (background.control.mosaic && mosaic.bgs.isMosaicX())
        {
            for (auto [x, color] : shell::enumerate(background.buffer))
            {
                color = background.buffer[mosaic.bgs.mosaicX(x)];
            }
        }
    }
}

template<uint kColorMode>
void Ppu::renderBackground0(Background& background)
{
    const auto size = background.control.sizeRegular();

    Point origin = (background.offset + Point(0, vcount)) % size;

    auto pixel = origin % kTileSize;
    auto tile  = origin / kTileSize % kMapBlockTiles;
    auto block = origin / kMapBlockSize;

    for (uint x = 0; ; block.x ^= size.x > kMapBlockSize)
    {
        u32 map = background.control.map_block
            + kMapBlockBytes * block.index2d(size.x / kMapBlockSize)
            + kMapEntryBytes * tile.index2d(kMapBlockTiles);

        for (; tile.x < kMapBlockTiles; ++tile.x, map += kMapEntryBytes)
        {
            MapEntry entry(vram.readFast<u16>(map));

            u32 addr = background.control.tile_block + kTileBytes[kColorMode] * entry.tile;
            if (addr < kObjectBase)
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    uint index = kColorMode == ColorMode::C16x16
                        ? vram.index16x16(addr, pixel ^ entry.flip)
                        : vram.index256x1(addr, pixel ^ entry.flip);

                    background.buffer[x] = kColorMode == ColorMode::C16x16
                        ? pram.colorBg(index, entry.bank)
                        : pram.colorBg(index);

                    if (++x == kScreenW)
                        return;
                }
            }
            else
            {
                for (; pixel.x < kTileSize; ++pixel.x)
                {
                    background.buffer[x] = kTransparent;

                    if (++x == kScreenW)
                        return;
                }
            }
            pixel.x = 0;
        }
        tile.x = 0;
    }
}

void Ppu::renderBackground0(Background& background)
{
    switch (background.control.color_mode)
    {
    SHELL_CASE02(0, renderBackground0<kLabel>(background))

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Ppu::renderBackground2(Background& background)
{
    const auto size = background.control.sizeAffine();

    for (auto [x, color] : shell::enumerate(background.buffer))
    {
        auto texel = background.matrix * x;

        if (static_cast<uint>(texel.x) >= size.x ||
            static_cast<uint>(texel.y) >= size.y)
        {
            if (background.control.wraparound)
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

        uint entry = vram.readFast<u8>(background.control.map_block + tile.index2d(size.x / kTileSize));
        uint index = vram.index256x1(background.control.tile_block + kTileBytes[uint(ColorMode::C256x1)] * entry, pixel);

        color = pram.colorBg(index);
    }
}

void Ppu::renderBackground3(Background& background)
{
    for (auto [x, color] : shell::enumerate(background.buffer))
    {
        const auto texel = background.matrix * x;

        if (static_cast<uint>(texel.x) < kScreenW &&
            static_cast<uint>(texel.y) < kScreenH)
        {
            u32 addr = kColorBytes * texel.index2d(kScreenW);

            color = vram.readFast<u16>(addr) & kColorMask;
        }
        else
        {
            color = kTransparent;
        }
    }
}

void Ppu::renderBackground4(Background& background)
{
    for (auto [x, color] : shell::enumerate(background.buffer))
    {
        const auto texel = background.matrix * x;

        if (static_cast<uint>(texel.x) < kScreenW &&
            static_cast<uint>(texel.y) < kScreenH)
        {
            uint index = vram.readFast<u8>(dispcnt.frame + texel.index2d(kScreenW));

            color = pram.colorBg(index);
        }
        else
        {
            color = kTransparent;
        }
    }
}

void Ppu::renderBackground5(Background& background)
{
    constexpr Point kBitmap(160, 128);

    for (auto [x, color] : shell::enumerate(background.buffer))
    {
        const auto texel = background.matrix * x;

        if (static_cast<uint>(texel.x) < kBitmap.x &&
            static_cast<uint>(texel.y) < kBitmap.y)
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
    s64 cycles = dispcnt.oam_free ? 954 : 1210;

    for (const auto& entry : oam.entries)
    {
        if (entry.disabled || !entry.isVisible(vcount))
            continue;

        const auto& origin      = entry.origin;
        const auto& center      = entry.center;
        const auto& sprite_size = entry.sprite_size;
        const auto& screen_size = entry.screen_size;
        const auto& matrix      = entry.affine ? oam.matrices[entry.matrix] : kIdentityMatrix;

        uint tile_bytes = entry.tileBytes();
        uint tiles_row  = entry.tilesPerRow(dispcnt.layout);
        uint bank       = entry.paletteBank();

        Point offset(
            -center.x + origin.x - std::min(origin.x, 0),
            -center.y + vcount);

        uint end = std::min<uint>(origin.x + screen_size.x, kScreenW);

        for (uint x = center.x + offset.x; x < end; ++x, ++offset.x)
        {
            auto texel = (matrix * offset) + (sprite_size / 2);

            if (static_cast<uint>(texel.x) >= sprite_size.x ||
                static_cast<uint>(texel.y) >= sprite_size.y)
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
                    if (entry.priority < object.priority || !object.isOpaque())
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
