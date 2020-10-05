#include "vram.h"

#include <shell/utility.h>

#include "base/bit.h"
#include "base/int.h"
#include "ppu/ppu.h"

void VRAM::reset()
{
    shell::reconstruct(*this);
}

void VRAM::writeByte(u32 addr, u8 byte)
{
    addr = mirror(addr);

    if (addr < (ppu.io.dispcnt.isBitmap() ? 0x1'4000u : 0x1'0000u))
    {
        addr = align<u16>(addr);

        writeFast<u16>(addr, byte * 0x0101);
    }
}

int VRAM::index(u32 addr, const Point& pixel, ColorMode mode)
{
    return mode == ColorMode::C256x1
        ? index256x1(addr, pixel)
        : index16x16(addr, pixel);
}

int VRAM::index256x1(u32 addr, const Point& pixel)
{
    return readFast<u8>(addr + pixel.offset(8));
}

int VRAM::index16x16(u32 addr, const Point& pixel)
{
    int data = readFast<u8>(addr + pixel.offset(8) / 2);

    return (pixel.x & 0x1)
        ? bit::seq<4, 4>(data)
        : bit::seq<0, 4>(data);
}
