#include "arm.h"

#include "mmu/mmu.h"

bool Arm::isSequential(u32 addr) const
{
    return (addr - prev_addr) <= 4;
}

u8 Arm::readByte(u32 addr)
{
    cycles -= wait_control.cyclesHalf(addr, isSequential(addr));
    prev_addr = addr;
    return mmu.readByte(addr);
}

u16 Arm::readHalf(u32 addr)
{
    cycles -= wait_control.cyclesHalf(addr, isSequential(addr));
    prev_addr = addr;
    return mmu.readHalf(addr);
}

u32 Arm::readWord(u32 addr)
{
    cycles -= wait_control.cyclesWord(addr, isSequential(addr));
    prev_addr = addr;
    return mmu.readWord(addr);
}

void Arm::writeByte(u32 addr, u8 byte)
{
    cycles -= wait_control.cyclesHalf(addr, isSequential(addr));
    prev_addr = addr;
    mmu.writeByte(addr, byte);
}

void Arm::writeHalf(u32 addr, u16 half)
{
    cycles -= wait_control.cyclesHalf(addr, isSequential(addr));
    prev_addr = addr;
    mmu.writeHalf(addr, half);
}

void Arm::writeWord(u32 addr, u32 word)
{
    cycles -= wait_control.cyclesWord(addr, isSequential(addr));
    prev_addr = addr;
    mmu.writeWord(addr, word);
}

u32 Arm::readWordRotate(u32 addr)
{
    u32 value = readWord(addr);

    return bit::ror(value, (addr & 0x3) << 3);
}

u32 Arm::readHalfRotate(u32 addr)
{
    u32 value = readHalf(addr);

    return bit::ror(value, (addr & 0x1) << 3);
}

u32 Arm::readHalfSignEx(u32 addr)
{
    if (addr & 0x1)
    {
        u32 value = readByte(addr);
        return bit::signEx<8>(value);
    }
    else
    {
        u32 value = readHalf(addr);
        return bit::signEx<16>(value);
    }
}
