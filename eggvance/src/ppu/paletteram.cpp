#include "paletteram.h"

#include "constants.h"

inline constexpr auto kBankBytes = 0x20;
inline constexpr auto kPaletteFg = 0x200;

void PaletteRam::writeByte(u32 addr, u8 byte)
{
    writeHalf(addr, byte * 0x0101);
}

u16 PaletteRam::colorFg(uint index, uint bank) const
{
    return index == 0
        ? kTransparent
        : colorFgOpaque(index, bank);
}

u16 PaletteRam::colorBg(uint index, uint bank) const
{
    return index == 0
        ? kTransparent
        : colorBgOpaque(index, bank);
}

u16 PaletteRam::colorFgOpaque(uint index, uint bank) const
{
    return readFast<u16>(kPaletteFg + kBankBytes * bank + kColorBytes * index) & kColorMask;
}

u16 PaletteRam::colorBgOpaque(uint index, uint bank) const
{
    return readFast<u16>(kBankBytes * bank + kColorBytes * index) & kColorMask;
}

u16 PaletteRam::backdrop() const
{
    return readFast<u16>(0) & kColorMask;
}
