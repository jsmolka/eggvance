#include "palette.h"

void Palette::reset()
{
    data.fill(0);
}

u8 Palette::readByte(u32 addr)
{
    return data.readByte(addr);
}

u16 Palette::readHalf(u32 addr)
{
    return data.readHalf(addr);
}

u32 Palette::readWord(u32 addr)
{
    return data.readWord(addr);
}

void Palette::writeByte(u32 addr, u8 byte)
{
    data.writeHalf(addr, byte * 0x0101);
}

void Palette::writeHalf(u32 addr, u16 half)
{
    data.writeHalf(addr, half);
}

void Palette::writeWord(u32 addr, u32 word)
{
    data.writeWord(addr, word);
}

u16 Palette::readColorFG(int index, int bank)
{
    if (index == 0)
        return transparent;
    else
        return *data.ptr<u16>(0x200 + 0x20 * bank + 2 * index);
}

u16 Palette::readColorBG(int index, int bank)
{
    if (index == 0)
        return transparent;
    else
        return *data.ptr<u16>(0x20 * bank + 2 * index);
}
