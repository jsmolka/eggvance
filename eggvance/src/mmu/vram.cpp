#include "vram.h"

#include "ppu/ppu.h"

void VRAM::reset()
{
    fill(0);
}

void VRAM::writeByte(u32 addr, u8 byte)
{
    if (ppu.io.dispcnt.mode < 3
            ? (addr - 0x600'0000) < 0x1'0000  
            : (addr - 0x600'0000) < 0x1'4000)
        writeHalf(addr, byte * 0x0101);
}

int VRAM::readPixel(u32 addr, int x, int y, Palette::Format format)
{
    if (format == Palette::Format::F16)
    {
        u8 byte = readByteFast(addr + 4 * y + x / 2);

        return (x & 0x1)
            ? (byte >> 4) & 0xF
            : (byte >> 0) & 0xF;
    }
    else
    {
        return readByteFast(addr + 8 * y + x);
    }
}

u32 VRAM::mirror(u32 addr) const
{
    if (addr >= 0x18000)
        addr -= 0x08000;
    return addr;
}
