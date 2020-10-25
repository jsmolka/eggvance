#include "vram.h"

#include "base/bit.h"
#include "base/int.h"
#include "gpu/gpu.h"
#include "gpu/constants.h"

void Vram::writeByte(u32 addr, u8 byte)
{
    addr = mirror(addr);

    if (addr < (gpu.dispcnt.isBitmap() ? 0x1'4000 : 0x1'0000))
    {
        addr = align<u16>(addr);

        writeFast<u16>(addr, byte * 0x0101);
    }
}

uint Vram::index256x1(u32 addr, const Point& pixel) const
{
    return readFast<u8>(addr + pixel.index2d(8));
}

uint Vram::index16x16(u32 addr, const Point& pixel) const
{
    u8 data = readFast<u8>(addr + pixel.index2d(8) / 2);

    return bit::nibble(data, pixel.x & 0x1);
}

uint Vram::index(u32 addr, const Point& pixel, uint mode) const
{
    return mode == kColorMode256x1
        ? index256x1(addr, pixel)
        : index16x16(addr, pixel);
}
