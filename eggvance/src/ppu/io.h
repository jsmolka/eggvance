#pragma once

#include <algorithm>

#include "layer.h"
#include "point.h"
#include "base/register.h"

class DisplayControl : public XRegister<u16, 0xFFF7>
{
public:
    DisplayControl();

    void write(uint index, u8 byte);

    bool isActive() const;
    bool isBitmap() const;

    uint mode     = 0;
    uint frame    = 0;
    uint oam_free = 0;
    uint layout   = 0;
    uint blank    = 0;
    uint layers   = 0;
    uint win0     = 0;
    uint win1     = 0;
    uint winobj   = 0;
};

class DisplayStatus : public XRegister<u16, 0xFF38>
{
public:
    u8 read(uint index) const;
    void write(uint index, u8 byte);

    uint vblank     = 0;
    uint hblank     = 0;
    uint vmatch     = 0;
    uint vblank_irq = 0;
    uint hblank_irq = 0;
    uint vmatch_irq = 0;
    uint vcompare   = 0;
};

class VCount : public XRegisterR<u16>
{
public:
    VCount& operator++();
    operator u16() const;
};

class BgControl : public XRegister<u16>
{
public:
    BgControl(uint id);

    void write(uint index, u8 byte);

    Point sizeReg() const;
    Point sizeAff() const;

    uint priority   = 0;
    uint tile_block = 0;
    uint mosaic     = 0;
    uint color_mode = 0;
    uint map_block  = 0;
    uint wraparound = 0;
    uint dimensions = 0;
};

class BgReference : public XRegisterW<u32, 0x0FFF'FFFF>
{
public:
    operator s32() const;

    void write(uint index, u8 byte);

    void hblank(s16 value);
    void vblank();

private:
    s32 current = 0;
};

class Window
{
public:
    Window();

    void write(u8 byte);

    uint flags = 0;
    uint blend = 0;
};

class WindowInside : public XRegister<u16, 0x3F3F>
{
public:
    void write(uint index, u8 byte);

    Window win0;
    Window win1;
};

class WindowOutside : public XRegister<u16, 0x3F3F>
{
public:
    void write(uint index, u8 byte);

    Window winout;
    Window winobj;
};

class WindowRange : public XRegisterW<u16>
{
public:
    WindowRange(uint limit);

    void write(uint index, u8 byte);

    bool contains(uint value) const;

    uint min = 0;
    uint max = 0;

private:
    uint limit = 0;
};

class Mosaic
{
public:
    class Block
    {
    public:
        void write(u8 byte)
        {
            x = bit::seq<0, 4>(byte) + 1;
            y = bit::seq<4, 4>(byte) + 1;
        }

        bool isDominantX(uint value) const { return value % x == 0; }
        bool isDominantY(uint value) const { return value % y == 0; }

        uint mosaicX(uint value) const { return x * (value / x); }
        uint mosaicY(uint value) const { return y * (value / y); }

        uint x = 1;
        uint y = 1;
    };

    template<uint Index>
    void write(u8 byte)
    {
        static_assert(Index < 2);

        if (Index == 0) bgs.write(byte);
        if (Index == 1) obj.write(byte);
    }

    Block bgs;
    Block obj;
};

class BlendControl : public Register<u16, 0x3FFF>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            upper = bit::seq<0, 6>(byte);
            mode  = bit::seq<6, 2>(byte);
        }
        else
        {
            lower = bit::seq<0, 6>(byte);
        }
    }

    uint mode  = 0;
    uint upper = 0;
    uint lower = 0;
};

class BlendAlpha : public Register<u16, 0x1F1F>
{
public:
    static constexpr uint kMaskR = 0x1F <<  0;
    static constexpr uint kMaskG = 0x1F <<  5;
    static constexpr uint kMaskB = 0x1F << 10;

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
            eva = std::min<uint>(16, bit::seq<0, 5>(byte));
        else
            evb = std::min<uint>(16, bit::seq<0, 5>(byte));
    }

    u16 blendAlpha(u16 a, u16 b) const
    {
        uint rr = std::min(kMaskR, ((a & kMaskR) * eva + (b & kMaskR) * evb) >> 4);
        uint gg = std::min(kMaskG, ((a & kMaskG) * eva + (b & kMaskG) * evb) >> 4);
        uint bb = std::min(kMaskB, ((a & kMaskB) * eva + (b & kMaskB) * evb) >> 4);

        return (rr & kMaskR) | (gg & kMaskG) | (bb & kMaskB);
    }

    uint eva = 0;
    uint evb = 0;
};

class BlendFade : public RegisterW<u16, 0x001F>
{
public:
    static constexpr uint kMaskR = 0x1F <<  0;
    static constexpr uint kMaskG = 0x1F <<  5;
    static constexpr uint kMaskB = 0x1F << 10;

    template<uint Index>
    void write(u8 byte)
    {
        RegisterW::write<Index>(byte);

        if (Index == 0)
            evy = std::min<uint>(16, bit::seq<0, 5>(byte));
    }

    u16 blendWhite(u16 a) const
    {
        uint r = a & kMaskR;
        uint g = a & kMaskG;
        uint b = a & kMaskB;

        r += ((kMaskR - r) * evy) >> 4;
        g += ((kMaskG - g) * evy) >> 4;
        b += ((kMaskB - b) * evy) >> 4;

        return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
    }

    u16 blendBlack(u16 a) const
    {
        uint r = a & kMaskR;
        uint g = a & kMaskG;
        uint b = a & kMaskB;

        r -= (r * evy) >> 4;
        g -= (g * evy) >> 4;
        b -= (b * evy) >> 4;

        return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
    }

    uint evy = 0;
};
