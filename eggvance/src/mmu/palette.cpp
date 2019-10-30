#include "palette.h"

#include "common/constants.h"

void Palette::reset()
{
    fill(0);
}

void Palette::writeByte(u32 addr, u8 byte)
{
    writeHalf(addr, byte * 0x0101);
}

u16 Palette::colorFG(int index, int bank)
{
    if (index == 0)
        return TRANSPARENT;
    else
        return colorFGOpaque(index, bank);
}

u16 Palette::colorBG(int index, int bank)
{
    if (index == 0)
        return TRANSPARENT;
    else
        return colorBGOpaque(index, bank);
}

u16 Palette::colorFGOpaque(int index, int bank)
{
    return readHalfFast(0x200 + 0x20 * bank + 2 * index);
}

u16 Palette::colorBGOpaque(int index, int bank)
{
    return readHalfFast(0x20 * bank + 2 * index);
}

u16 Palette::backdrop()
{
    return readHalfFast(0);
}
