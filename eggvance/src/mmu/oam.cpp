#include "oam.h"

#include "base/bit.h"

void Oam::writeHalf(u32 addr, u16 half)
{
    addr = mirror(addr);
    addr = align<u16>(addr);

    uint attr = addr & 0x6;
    if ( attr < 0x6)
        entries[addr >> 3].write(attr, half);

    writeFast<u16>(addr, half);
}

void Oam::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    writeHalf(addr + 0, bit::seq< 0, 16>(word));
    writeHalf(addr + 2, bit::seq<16, 16>(word));
}

Matrix Oam::matrix(uint index) const
{
    enum Parameter
    {
        kParameterA = 0x06,
        kParameterB = 0x0E,
        kParameterC = 0x16,
        kParameterD = 0x1E
    };

    u32 base = 0x20 * index;

    return Matrix(
        readFast<u16>(base + kParameterA),
        readFast<u16>(base + kParameterB),
        readFast<u16>(base + kParameterC),
        readFast<u16>(base + kParameterD));
}
