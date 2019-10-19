#include "arm.h"

u32 ARM::readWordRotated(u32 addr)
{
    u32 value = mmu.readWord(addr);
    if (misalignedWord(addr))
    {
        int rotation = (addr & 0x3) << 3;
        value = ror(value, rotation, true);
    }
    return value;
}

u32 ARM::readHalfRotated(u32 addr)
{
    u32 value = mmu.readHalf(addr);
    if (misalignedHalf(addr))
    {
        value = ror(value, 8, true);
    }
    return value;
}

u32 ARM::readHalfSigned(u32 addr)
{
    u32 value = 0;
    if (misalignedHalf(addr))
    {
        value = mmu.readByte(addr);
        value = signExtend<8>(value);
    }
    else
    {
        value = mmu.readHalf(addr);
        value = signExtend<16>(value);
    }
    return value;
}
