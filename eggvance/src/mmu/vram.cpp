#include "vram.h"

#include "ppu/ppu.h"

void VRAM::reset()
{
    data.fill(0);
}

u8 VRAM::readByte(u32 addr)
{
    return data.readByte(addr);
}

u16 VRAM::readHalf(u32 addr)
{
    return data.readHalf(addr);
}

u32 VRAM::readWord(u32 addr)
{
    return data.readWord(addr);
}

void VRAM::writeByte(u32 addr, u8 byte)
{
    addr &= 0x17'FFE;
    if (ppu.io.dispcnt.mode < 3
            ? addr < 0x1'0000  
            : addr < 0x1'4000)
        data.writeHalf(addr, byte * 0x0101);
}

void VRAM::writeHalf(u32 addr, u16 half)
{
    data.writeHalf(addr, half);
}

void VRAM::writeWord(u32 addr, u32 word)
{
    data.writeWord(addr, word);
}

u16 VRAM::readPixel(u32 addr, int x, int y, Palette::Format format)
{
    if (format == Palette::Format::F16)
    {
        u8 byte = data.readByteFast(addr + 4 * y + x / 2);

        return (x & 0x1)
            ? (byte >> 4) & 0xF
            : (byte >> 0) & 0xF;
    }
    else
    {
        return data.readByteFast(addr + 8 * y + x);
    }
}
