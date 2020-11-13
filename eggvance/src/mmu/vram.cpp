#include "vram.h"

#include "base/bit.h"
#include "gpu/gpu.h"
#include "gpu/constants.h"

u32 VramMirror::operator()(u32 addr) const
{
    addr &= 0x1'FFFF;

    if (addr >= 0x1'8000)
        addr -= 0x0'8000;

    return addr;
}

void Vram::writeByte(u32 addr, u8 byte)
{
    addr = mirror(addr);

    if (addr < (gpu.dispcnt.isBitmap() ? kObjectBaseBitmap : kObjectBase))
    {
        addr = align<u16>(addr);

        writeFast<u16>(addr, byte * 0x0101);
    }
}

uint Vram::index256x1(u32 addr, const Point& pixel) const
{
    return readFast<u8>(addr + pixel.index2d(kTileSize));
}

uint Vram::index16x16(u32 addr, const Point& pixel) const
{
    u8 data = readFast<u8>(addr + pixel.index2d(kTileSize) / 2);

    return bit::nibble(data, pixel.x & 0x1);
}

uint Vram::index(u32 addr, const Point& pixel, uint mode) const
{
    return mode == kColorMode16x16
        ? index16x16(addr, pixel)
        : index256x1(addr, pixel);
}
