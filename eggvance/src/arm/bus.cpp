#include "arm.h"

#include "common/bits.h"
#include "mmu/mmu.h"

bool ARM::isSequential(u32 addr) const
{
    return (addr - last_addr) <= 4;
}

u8 ARM::readByte(u32 addr)
{
    remaining -= io.waitcnt.cyclesHalf(addr, isSequential(addr));
    last_addr = addr;
    return mmu.readByte(addr);
}

u16 ARM::readHalf(u32 addr)
{
    remaining -= io.waitcnt.cyclesHalf(addr, isSequential(addr));
    last_addr = addr;
    return mmu.readHalf(addr);
}

u32 ARM::readWord(u32 addr)
{
    remaining -= io.waitcnt.cyclesWord(addr, isSequential(addr));
    last_addr = addr;
    return mmu.readWord(addr);
}

void ARM::writeByte(u32 addr, u8 byte)
{
    remaining -= io.waitcnt.cyclesHalf(addr, isSequential(addr));
    last_addr = addr;
    mmu.writeByte(addr, byte);
}

void ARM::writeHalf(u32 addr, u16 half)
{
    remaining -= io.waitcnt.cyclesHalf(addr, isSequential(addr));
    last_addr = addr;
    mmu.writeHalf(addr, half);
}

void ARM::writeWord(u32 addr, u32 word)
{
    remaining -= io.waitcnt.cyclesWord(addr, isSequential(addr));
    last_addr = addr;
    mmu.writeWord(addr, word);
}

u32 ARM::readWordRotated(u32 addr)
{
    u32 value = readWord(addr);

    return rotateRight(value, (addr & 0x3) << 3);
}

u32 ARM::readHalfRotated(u32 addr)
{
    u32 value = readHalf(addr);

    return rotateRight(value, (addr & 0x1) << 3);
}

u32 ARM::readHalfSigned(u32 addr)
{
    if (addr & 0x1)
    {
        u32 value = readByte(addr);
        return signExtend<8>(value);
    }
    else
    {
        u32 value = readHalf(addr);
        return signExtend<16>(value);
    }
}
