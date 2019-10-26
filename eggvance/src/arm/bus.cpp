#include "arm.h"

#include "common/utility.h"
#include "mmu/mmu.h"

u8 ARM::readByte(u32 addr)
{
    cycles -= io.waitcnt.cyclesHalf(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    return mmu.readByte(addr);
}

u16 ARM::readHalf(u32 addr)
{
    cycles -= io.waitcnt.cyclesHalf(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    return mmu.readHalf(addr);
}

u32 ARM::readWord(u32 addr)
{
    cycles -= io.waitcnt.cyclesWord(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    return mmu.readWord(addr);
}

void ARM::writeByte(u32 addr, u8 byte)
{
    cycles -= io.waitcnt.cyclesHalf(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    mmu.writeByte(addr, byte);

}

void ARM::writeHalf(u32 addr, u16 half)
{
    cycles -= io.waitcnt.cyclesHalf(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    mmu.writeHalf(addr, half);
}

void ARM::writeWord(u32 addr, u32 word)
{
    cycles -= io.waitcnt.cyclesWord(addr, (addr - last_addr) <= 4);
    last_addr = addr;
    mmu.writeWord(addr, word);
}

u32 ARM::readWordRotated(u32 addr)
{
    u32 value = readWord(addr);
    if (misalignedWord(addr))
    {
        int rotation = (addr & 0x3) << 3;
        value = ror(value, rotation, true);
    }
    return value;
}

u32 ARM::readHalfRotated(u32 addr)
{
    u32 value = readHalf(addr);
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
        value = readByte(addr);
        value = signExtend<8>(value);
    }
    else
    {
        value = readHalf(addr);
        value = signExtend<16>(value);
    }
    return value;
}
