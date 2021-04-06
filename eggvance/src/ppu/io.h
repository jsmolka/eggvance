#pragma once

#include "point.h"
#include "base/register.h"

class DisplayControl : public Register<u16, 0xFFF7>
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

class DisplayStatus : public Register<u16, 0xFF38>
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

class VCount : public RegisterR<u16>
{
public:
    VCount& operator++();
    operator u16() const;
};

class BgControl : public Register<u16>
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

class BgReference : public RegisterW<u32, 0x0FFF'FFFF>
{
public:
    operator s32() const;

    void write(uint index, u8 byte);

    void hblank(s16 value);
    void vblank();

private:
    s32 current = 0;
};

class BgParameter : public RegisterW<u16>
{
public:
    operator s16() const;
};

class BgOffset : public RegisterW<u16, 0x01FF>
{
public:
    operator u16() const;
};

class Window
{
public:
    enum Flag
    {
        Zero = 1 << 0,
        One  = 1 << 1,
        Obj  = 1 << 2
    };

    Window();

    void write(u8 byte);

    uint layers = 0;
    uint blend  = 0;
};

class WindowInside : public Register<u16, 0x3F3F>
{
public:
    void write(uint index, u8 byte);

    Window win0;
    Window win1;
};

class WindowOutside : public Register<u16, 0x3F3F>
{
public:
    void write(uint index, u8 byte);

    Window winout;
    Window winobj;
};

class WindowRange : public RegisterW<u16>
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
        void write(u8 byte);

        uint mosaicX(uint value) const;
        uint mosaicY(uint value) const;

        bool isDominantX(uint value) const;
        bool isDominantY(uint value) const;

        uint x = 1;
        uint y = 1;
    };

    void write(uint index, u8 byte);

    Block bgs;
    Block obj;
};

class BlendControl : public Register<u16, 0x3FFF>
{
public:
    void write(uint index, u8 byte);

    uint mode  = 0;
    uint upper = 0;
    uint lower = 0;
};

class BlendAlpha : public Register<u16, 0x1F1F>
{
public:
    void write(uint index, u8 byte);

    u16 blendAlpha(u16 a, u16 b) const;

    uint eva = 0;
    uint evb = 0;
};

class BlendFade : public RegisterW<u16, 0x001F>
{
public:
    void write(uint index, u8 byte);

    u16 blendWhite(u16 a) const;
    u16 blendBlack(u16 a) const;

    uint evy = 0;
};
