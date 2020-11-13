#pragma once

#include "gpu/point.h"
#include "ram.h"

class VramMirror
{
public:
    u32 operator()(u32 addr) const;
};

class Vram : public Ram<0x1'8000, VramMirror>
{
public:
    void writeByte(u32 addr, u8 byte);

    uint index256x1(u32 addr, const Point& pixel) const;
    uint index16x16(u32 addr, const Point& pixel) const;
    uint index(u32 addr, const Point& pixel, uint mode) const;
};
