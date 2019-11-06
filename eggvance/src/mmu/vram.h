#pragma once

#include "palette.h"
#include "ram.h"

class VRAM : public RAM<0x1'8000, 0x2'0000>
{
public:
    void reset();

    void writeByte(u32 addr, u8 byte);

    int readPixel(u32 addr, int x, int y, Palette::Format format);

private:
    u32 mirror(u32 addr) const override final;
};
