#include "io.h"

#include <algorithm>
#include <shell/operators.h>

#include "layers.h"
#include "base/config.h"

inline constexpr auto kMaskR = 0x1F <<  0;
inline constexpr auto kMaskG = 0x1F <<  5;
inline constexpr auto kMaskB = 0x1F << 10;

DisplayControl::DisplayControl()
{
    if (config.bios_skip)
    {
        write(0, 0x80);
    }
}

void DisplayControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
    {
        mode     = bit::seq<0, 3>(byte);
        frame    = bit::seq<4, 1>(byte) * kFrameBytes;
        oam_free = bit::seq<5, 1>(byte);
        layout   = bit::seq<6, 1>(byte);
        blank    = bit::seq<7, 1>(byte);
    }
    else
    {
        layers = bit::seq<0, 5>(byte);
        win0   = bit::seq<5, 1>(byte);
        win1   = bit::seq<6, 1>(byte);
        winobj = bit::seq<7, 1>(byte);
    }
}

bool DisplayControl::isActive() const
{
    static constexpr uint kLayers[8] =
    {
        uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2 | Layer::Flag::Bg3 | Layer::Flag::Obj),
        uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2 | Layer::Flag::Obj),
        uint(Layer::Flag::Bg2 | Layer::Flag::Bg3 | Layer::Flag::Obj),
        uint(Layer::Flag::Bg2 | Layer::Flag::Obj),
        uint(Layer::Flag::Bg2 | Layer::Flag::Obj),
        uint(Layer::Flag::Bg2 | Layer::Flag::Obj),
        uint(0),
        uint(0)
    };
    return kLayers[mode] & layers;
}

bool DisplayControl::isBitmap() const
{
    return mode > 2;
}

u8 DisplayStatus::read(uint index) const
{
    u8 value = Register::read(index);

    if (index == 0)
    {
        value |= vblank << 0;
        value |= hblank << 1;
        value |= vmatch << 2;
    }
    return value;
}

void DisplayStatus::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
    {
        vblank_irq = bit::seq<3, 1>(byte);
        hblank_irq = bit::seq<4, 1>(byte);
        vmatch_irq = bit::seq<5, 1>(byte);
    }
    else
    {
        vcompare = byte;
    }
}

VCount& VCount::operator++()
{
    data = (data + 1) % 228;
    return *this;
}

VCount::operator u16() const
{
    return data;
}

BackgroundControl::BackgroundControl(uint id)
    : Register(id <= 1 ? 0xDFFF : 0xFFFF)
{

}

void BackgroundControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
    {
        priority   = bit::seq<0, 2>(byte);
        tile_block = bit::seq<2, 2>(byte) * kTileBlockBytes;
        mosaic     = bit::seq<6, 1>(byte);
        color_mode = bit::seq<7, 1>(byte);
    }
    else
    {
        map_block  = bit::seq<0, 5>(byte) * kMapBlockBytes;
        wraparound = bit::seq<5, 1>(byte);
        dimensions = bit::seq<6, 2>(byte);
    }
}

Point BackgroundControl::sizeRegular() const
{
    return Point(
        256 << bit::seq<0, 1>(dimensions),
        256 << bit::seq<1, 1>(dimensions));
}

Point BackgroundControl::sizeAffine() const
{
    return Point(
        128 << dimensions,
        128 << dimensions);
}

void BackgroundOffset::writeX(uint index, u8 byte)
{
    bit::byteRef(x, index) = byte;
    x &= 0x1FF;
}

void BackgroundOffset::writeY(uint index, u8 byte)
{
    bit::byteRef(y, index) = byte;
    y &= 0x1FF;
}

Window::Window()
{
    write(0);
}

void Window::write(u8 byte)
{
    layers = bit::seq<0, 5>(byte) | Layer::Flag::Bdp;
    blend  = bit::seq<5, 1>(byte);
}

void WindowInside::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
        win0.write(byte);
    else
        win1.write(byte);
}

void WindowOutside::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
        winout.write(byte);
    else
        winobj.write(byte);
}

WindowRange::WindowRange(uint limit)
    : limit(limit)
{

}

void WindowRange::write(uint index, u8 byte)
{
    RegisterW::write(index, byte);

    max = bit::byte(data, 0);
    min = bit::byte(data, 1);

    if (max > limit || max < min)
        max = limit;
}

bool WindowRange::contains(uint value) const
{
    return value >= min && value < max;
}

void Mosaic::Block::write(u8 byte)
{
    x = bit::seq<0, 4>(byte) + 1;
    y = bit::seq<4, 4>(byte) + 1;
}

uint Mosaic::Block::mosaicX(uint value) const
{
    return x * (value / x);
}

uint Mosaic::Block::mosaicY(uint value) const
{
    return y * (value / y);
}

bool Mosaic::Block::isDominantX(uint value) const
{
    return value % x == 0;
}

bool Mosaic::Block::isDominantY(uint value) const
{
    return value % y == 0;
}

void Mosaic::write(uint index, u8 byte)
{
    SHELL_ASSERT(index < 2);

    if (index == 0)
        bgs.write(byte);
    else
        obj.write(byte);
}

void BlendControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
    {
        upper = bit::seq<0, 6>(byte);
        mode  = bit::seq<6, 2>(byte);
    }
    else
    {
        lower = bit::seq<0, 6>(byte);
    }
}

void BlendAlpha::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
        eva = std::min<uint>(16, bit::seq<0, 5>(byte));
    else
        evb = std::min<uint>(16, bit::seq<0, 5>(byte));
}

u16 BlendAlpha::blendAlpha(u16 a, u16 b) const
{
    uint rr = std::min<uint>(kMaskR, ((a & kMaskR) * eva + (b & kMaskR) * evb) >> 4);
    uint gg = std::min<uint>(kMaskG, ((a & kMaskG) * eva + (b & kMaskG) * evb) >> 4);
    uint bb = std::min<uint>(kMaskB, ((a & kMaskB) * eva + (b & kMaskB) * evb) >> 4);

    return (rr & kMaskR) | (gg & kMaskG) | (bb & kMaskB);
}

void BlendFade::write(uint index, u8 byte)
{
    if (index == 1)
        return;

    RegisterW::write(index, byte);

    evy = std::min<uint>(16, bit::seq<0, 5>(byte));
}

u16 BlendFade::blendWhite(u16 a) const
{
    uint r = a & kMaskR;
    uint g = a & kMaskG;
    uint b = a & kMaskB;

    r += ((kMaskR - r) * evy) >> 4;
    g += ((kMaskG - g) * evy) >> 4;
    b += ((kMaskB - b) * evy) >> 4;

    return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
}

u16 BlendFade::blendBlack(u16 a) const
{
    uint r = a & kMaskR;
    uint g = a & kMaskG;
    uint b = a & kMaskB;

    r -= (r * evy) >> 4;
    g -= (g * evy) >> 4;
    b -= (b * evy) >> 4;

    return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
}
