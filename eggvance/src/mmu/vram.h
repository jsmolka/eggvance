#pragma once

#include "ram.h"
#include "base/constants.h"
#include "ppu/point.h"

class VRAM : public RAM<VRAM, 0x1'8000>
{
public:
    void reset();

    void writeByte(u32 addr, u8 byte);

    int index(u32 addr, const Point& pixel, ColorMode mode);
    int index256x1(u32 addr, const Point& pixel);
    int index16x16(u32 addr, const Point& pixel);

    static inline u32 mirror(u32 addr)
    {
        addr &= 0x1'FFFF;

        if (addr >= 0x1'8000)
            addr -= 0x0'8000;

        return addr;
    }
};
