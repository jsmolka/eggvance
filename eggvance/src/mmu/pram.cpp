#include "pram.h"

#include "gpu/constants.h"

constexpr uint kBankBytes = 0x20;
constexpr uint kPaletteFg = 0x200;

void Pram::writeByte(u32 addr, u8 byte)
{
    writeHalf(addr, byte * 0x0101);
}

u16 Pram::colorFg(uint index, uint bank) const
{
    return index == 0
        ? kTransparent
        : colorFgOpaque(index, bank);
}

u16 Pram::colorBg(uint index, uint bank) const
{
    return index == 0
        ? kTransparent
        : colorBgOpaque(index, bank);
}

u16 Pram::colorFgOpaque(uint index, uint bank) const
{
    return readFast<u16>(kPaletteFg + kBankBytes * bank + kColorBytes * index) & kColorMask;
}

u16 Pram::colorBgOpaque(uint index, uint bank) const
{
    return readFast<u16>(kBankBytes * bank + kColorBytes * index) & kColorMask;
}

u16 Pram::backdrop() const
{
    return readFast<u16>(0) & kColorMask;
}
