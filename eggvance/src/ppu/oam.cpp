#include "oam.h"

#include <shell/macros.h>

#include "base/bit.h"

void Oam::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;
    addr = mirror(addr);

    auto& entry = entries[addr >> 3];
    auto& matrix = matrices[addr >> 5];

    switch (addr & 0x6)
    {
    case 0: entry.writeAttr0(half); break;
    case 2: entry.writeAttr1(half); break;
    case 4: entry.writeAttr2(half); break;
    case 6: matrix.write(addr >> 3, half); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    writeFast<u16>(addr, half);
}

void Oam::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;

    writeHalf(addr + 0, bit::seq< 0, 16>(word));
    writeHalf(addr + 2, bit::seq<16, 16>(word));
}
