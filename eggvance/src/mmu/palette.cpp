#include "palette.h"

#include "common/constants.h"

void Palette::reset()
{
    *this = Palette();
}

void Palette::writeByte(u32 addr, u8 byte)
{
    writeHalf(addr, byte * 0x0101);
}

u16 Palette::colorFG(int index, int bank)
{
    return index == 0
        ? TRANSPARENT
        : colorFGOpaque(index, bank);
}

u16 Palette::colorBG(int index, int bank)
{
    return index == 0
        ? TRANSPARENT
        : colorBGOpaque(index, bank);
}

u16 Palette::colorFGOpaque(int index, int bank)
{
    return readFast<u16>(0x200 + 0x20 * bank + 2 * index) & COLOR_MASK;
}

u16 Palette::colorBGOpaque(int index, int bank)
{
    return readFast<u16>(0x20 * bank + 2 * index) & COLOR_MASK;
}

u16 Palette::backdrop()
{
    return readFast<u16>(0) & COLOR_MASK;
}
