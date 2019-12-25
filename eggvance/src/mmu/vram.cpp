#include "vram.h"

#include "common/bits.h"
#include "ppu/ppu.h"

void VRAM::reset()
{
    fill(0);
}

void VRAM::writeByte(u32 addr, u8 byte)
{
    addr = mirror(addr);

    if (addr < (ppu.io.dispcnt.isBitmap() ? 0x1'4000u : 0x1'0000u))
    {
        addr = align<u16>(addr);

        writeHalfFast(addr, byte * 0x0101);
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
    return readByteFast(addr + pixel.offset(8));
}

int VRAM::index16x16(u32 addr, const Point& pixel)
{
    int data = readByteFast(addr + pixel.offset(8) / 2);

    return (pixel.x & 0x1)
        ? bits<4, 4>(data)
        : bits<0, 4>(data);
}

u32 VRAM::mirror(u32 addr) const
{
    addr &= 0x1'FFFF;

    if (addr >= 0x1'8000)
        addr -= 0x0'8000;

    return addr;
}
