#include "oam.h"

#include "common/bits.h"

void OAM::reset()
{
    *this = OAM();
}

void OAM::writeHalf(u32 addr, u16 half)
{
    addr = mirror(addr);
    addr = align<u16>(addr);

    int attr = addr & 0x6;
    if (attr < 0x6)
        entries[addr >> 3].writeHalf(attr, half);

    writeFast<u16>(addr, half);
}

void OAM::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    writeHalf(addr + 0, bits< 0, 16>(word));
    writeHalf(addr + 2, bits<16, 16>(word));
}

Matrix OAM::matrix(int index)
{
    return Matrix(
        readFast<u16>(0x20 * index + 0x06),
        readFast<u16>(0x20 * index + 0x0E),
        readFast<u16>(0x20 * index + 0x16),
        readFast<u16>(0x20 * index + 0x1E)
    );
}
