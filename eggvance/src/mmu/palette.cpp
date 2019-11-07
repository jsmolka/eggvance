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

void Palette::writeHalf(u32 addr, u16 half)
{
    addr = align<u16>(addr);

    colors[addr >> 1] = half & 0x7FFF;
    
    writeHalfFast(addr, half);
}

void Palette::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    colors[(addr >> 1) + 0] = (word >>  0) & 0x7FFF;
    colors[(addr >> 1) + 1] = (word >> 16) & 0x7FFF;

    writeWordFast(addr, word);
}

u16 Palette::colorFG(int index, int bank) const
{
    if (index == 0)
        return TRANSPARENT;
    else
        return colorFGOpaque(index, bank);
}

u16 Palette::colorBG(int index, int bank) const
{
    if (index == 0)
        return TRANSPARENT;
    else
        return colorBGOpaque(index, bank);
}

u16 Palette::colorFGOpaque(int index, int bank) const
{
    return colors[0x100 + 0x10 * bank + index];
}

u16 Palette::colorBGOpaque(int index, int bank) const
{
    return colors[0x10 * bank + index];
}

u16 Palette::backdrop() const
{
    return colors[0];
}
