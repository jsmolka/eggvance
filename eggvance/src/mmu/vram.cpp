#include "vram.h"

#include "ppu/ppu.h"

void VRAM::reset()
{
    fill(0);
}

void VRAM::writeByte(u32 addr, u8 byte)
{
    addr &= 0x17'FFE;
    if (ppu.io.dispcnt.mode < 3
            ? addr < 0x1'0000  
            : addr < 0x1'4000)
        writeHalf(addr, byte * 0x0101);
}

int VRAM::readPixel(u32 addr, int x, int y, Palette::Format format)
{
    if (format == Palette::Format::F16)
    {
        u8 byte = RAM::readByteFast(addr + 4 * y + x / 2);

        return (x & 0x1)
            ? (byte >> 4) & 0xF
            : (byte >> 0) & 0xF;
    }
    else
    {
        return RAM::readByteFast(addr + 8 * y + x);
    }
}

u32 VRAM::mirror(u32 addr) const
{
    if (addr >= 0x1'8000)
        addr -= 0x8000;
    return addr;
}
