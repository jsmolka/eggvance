#include "videoram.h"

#include <shell/operators.h>

#include "constants.h"
#include "ppu.h"
#include "base/bit.h"

u32 VideoRamMirror::operator()(u32 addr) const
{
    return addr & ((addr & 0x1'0000) ? 0x1'7FFF : 0x0'FFFF);
}

void VideoRam::writeByte(u32 addr, u8 byte)
{
    addr = mirror(addr);

    if (addr < (ppu.dispcnt.isBitmap() ? kObjectBaseBitmap : kObjectBase))
    {
        writeFast<u16>(addr & ~0x1, byte * 0x0101);
    }
}

uint VideoRam::index256x1(u32 addr, const Point& pixel) const
{
    return readFast<u8>(addr + pixel.index2d(kTileSize));
}

uint VideoRam::index16x16(u32 addr, const Point& pixel) const
{
    u8 data = readFast<u8>(addr + pixel.index2d(kTileSize) / 2);

    return bit::nibble(data, pixel.x & 0x1);
}

uint VideoRam::index(u32 addr, const Point& pixel, uint mode) const
{
    return mode == mode == ColorMode::C16x16
        ? index16x16(addr, pixel)
        : index256x1(addr, pixel);
}
