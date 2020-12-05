#include "arm.h"

#include "mmu/constants.h"
#include "mmu/mmu.h"
#include "timer/timer.h"

u32 Arm::readUnused() const
{
    u32 value = arm.pipe[1];

    if (arm.cpsr.t)
    {
        switch (arm.pc >> 24)
        {
        case kRegionBios:
        case kRegionOam:
            value <<= 16;
            value |= arm.pipe[0];
            break;

        case kRegionInternalWorkRam:
            if (arm.pc & 0x2)
            {
                value <<= 16;
                value |= arm.pipe[0];
            }
            else
            {
                value |= arm.pipe[0] << 16;
            }
            break;

        default:
            value |= value << 16;
            break;
        }
    }
    return value;
}

u8 Arm::readByte(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesHalf(addr, access));
    return mmu.readByte(addr);
}

u16 Arm::readHalf(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesHalf(addr, access));
    return mmu.readHalf(addr);
}

u32 Arm::readWord(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesWord(addr, access));
    return mmu.readWord(addr);
}

void Arm::writeByte(u32 addr, u8 byte, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesHalf(addr, access));
    mmu.writeByte(addr, byte);
}

void Arm::writeHalf(u32 addr, u16 half, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesHalf(addr, access));
    mmu.writeHalf(addr, half);
}

void Arm::writeWord(u32 addr, u32 word, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesWord(addr, access));
    mmu.writeWord(addr, word);
}

u32 Arm::readWordRotate(u32 addr, Access access)
{
    u32 value = readWord(addr, access);
    return bit::ror(value, 8 * (addr & 0x3));
}

u32 Arm::readHalfRotate(u32 addr, Access access)
{
    u32 value = readHalf(addr, access);
    return bit::ror(value, 8 * (addr & 0x1));
}

u32 Arm::readHalfSignEx(u32 addr, Access access)
{
    if (addr & 0x1)
    {
        u32 value = readByte(addr, access);
        return bit::signEx<8>(value);
    }
    else
    {
        u32 value = readHalf(addr, access);
        return bit::signEx<16>(value);
    }
}
