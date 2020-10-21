#pragma once

#include "ram.h"
#include "gpu/point.h"

class VramMirror
{
public:
    u32 operator()(u32 addr) const
    {
        addr &= 0x1'FFFF;

        if (addr >= 0x1'8000)
            addr -= 0x0'8000;

        return addr;
    }
};

class Vram : public Ram<0x1'8000, VramMirror>
{
public:
    void writeByte(u32 addr, u8 byte);

    uint index256x1(u32 addr, const Point& pixel) const;
    uint index16x16(u32 addr, const Point& pixel) const;
    uint index(u32 addr, const Point& pixel, uint mode) const;
};
