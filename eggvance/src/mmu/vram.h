#pragma once

#include "ram.h"
#include "common/enums.h"
#include "ppu/point.h"

class VRAM : public RAM<0x1'8000>
{
public:
    void reset();

    void writeByte(u32 addr, u8 byte);

    int index(u32 addr, const Point& pixel, ColorMode mode);
    int index256x1(u32 addr, const Point& pixel);
    int index16x16(u32 addr, const Point& pixel);

    u32 mirror(u32 addr) const override final;
};
