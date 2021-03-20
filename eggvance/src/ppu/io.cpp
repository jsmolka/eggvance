#include "io.h"

#include "base/config.h"

inline constexpr uint kMaskR = 0x1F <<  0;
inline constexpr uint kMaskG = 0x1F <<  5;
inline constexpr uint kMaskB = 0x1F << 10;

DisplayControl::DisplayControl()
{
    if (config.bios_skip)
    {
        write(0, 0x80);
        write(1, 0x00);
    }
}

void DisplayControl::write(uint index, u8 byte)
{
    if (!Register::write(index, byte))
        return;

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
        kLayerBg0 | kLayerBg1 | kLayerBg2 | kLayerBg3 | kLayerObj,
        kLayerBg0 | kLayerBg1 | kLayerBg2 | kLayerObj,
        kLayerBg2 | kLayerBg3 | kLayerObj,
        kLayerBg2 | kLayerObj,
        kLayerBg2 | kLayerObj,
        kLayerBg2 | kLayerObj,
        0, 0
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
    if (!Register::write(index, byte))
        return;

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
    value = (value + 1) % 228;

    return *this;
}

VCount::operator u16() const
{
    return value;
}

BgControl::BgControl(uint id)
    : Register(id <= 1 ? 0xDFFF : 0xFFFF)
{

}

void BgControl::write(uint index, u8 byte)
{
    if (!Register::write(index, byte))
        return;

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

Point BgControl::sizeReg() const
{
    return Point(
        256 << bit::seq<0, 1>(dimensions),
        256 << bit::seq<1, 1>(dimensions));
}

Point BgControl::sizeAff() const
{
    return Point(
        128 << dimensions,
        128 << dimensions);
}

BgReference::operator s32() const
{
    return current;
}

void BgReference::write(uint index, u8 byte)
{
    RegisterW::write(index, byte);

    value &= mask;

    vblank();
}

void BgReference::hblank(s16 value)
{
    current += value;
}

void BgReference::vblank()
{
    current = bit::signEx<28>(value);
}

Window::Window()
{
    write(0);
}

void Window::write(u8 byte)
{
    flags = bit::seq<0, 5>(byte) | kLayerBdp;
    blend = bit::seq<5, 1>(byte);
}

void WindowInside::write(uint index, u8 byte)
{
    if (!Register::write(index, byte))
        return;

    if (index == 0)
        win0.write(byte);
    else
        win1.write(byte);
}

void WindowOutside::write(uint index, u8 byte)
{
    if (!Register::write(index, byte))
        return;

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
    if (!RegisterW::write(index, byte))
        return;
    
    max = bytes[0];
    min = bytes[1];

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
    if (!Register::write(index, byte))
        return;

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
    if (!Register::write(index, byte))
        return;

    if (index == 0)
        eva = std::min<uint>(16, bit::seq<0, 5>(byte));
    else
        evb = std::min<uint>(16, bit::seq<0, 5>(byte));
}

u16 BlendAlpha::blendAlpha(u16 a, u16 b) const
{
    uint rr = std::min(kMaskR, ((a & kMaskR) * eva + (b & kMaskR) * evb) >> 4);
    uint gg = std::min(kMaskG, ((a & kMaskG) * eva + (b & kMaskG) * evb) >> 4);
    uint bb = std::min(kMaskB, ((a & kMaskB) * eva + (b & kMaskB) * evb) >> 4);

    return (rr & kMaskR) | (gg & kMaskG) | (bb & kMaskB);
}

void BlendFade::write(uint index, u8 byte)
{
    if (index == 1 || !RegisterW::write(index, byte))
        return;

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
