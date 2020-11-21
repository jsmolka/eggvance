#include "oam.h"

#include "base/bit.h"
#include "base/macros.h"

void Oam::writeHalf(u32 addr, u16 half)
{
    addr = mirror(addr);
    addr = align<u16>(addr);

    auto& entry = entries[addr >> 3];
    auto& matrix = matrices[addr >> 5];

    switch (addr & 0x6)
    {
    case 0: entry.writeAttr0(half); break;
    case 2: entry.writeAttr1(half); break;
    case 4: entry.writeAttr2(half); break;
    case 6: matrix[bit::seq<3, 2>(addr)] = half; break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    writeFast<u16>(addr, half);
}

void Oam::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    writeHalf(addr + 0, bit::seq< 0, 16>(word));
    writeHalf(addr + 2, bit::seq<16, 16>(word));
}
