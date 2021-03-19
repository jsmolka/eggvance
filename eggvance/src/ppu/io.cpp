#include "io.h"

#include "base/config.h"

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
    if (!XRegister::write(index, byte))
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
    u8 value = XRegister::read(index);

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
    if (!XRegister::write(index, byte))
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
    : XRegister(id <= 1 ? 0xDFFF : 0xFFFF)
{

}

void BgControl::write(uint index, u8 byte)
{
    if (!XRegister::write(index, byte))
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
    XRegisterW::write(index, byte);

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
    if (!XRegister::write(index, byte))
        return;

    if (index == 0)
        win0.write(byte);
    else
        win1.write(byte);
}

void WindowOutside::write(uint index, u8 byte)
{
    if (!XRegister::write(index, byte))
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
    if (!XRegisterW::write(index, byte))
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
