#include "oam.h"
#include "common/macros.h"

void OAM::reset()
{
    data.fill(0);
    
    for (auto& entry : entries)
    {
        entry.reset();
    }
}

const OAMEntry& OAM::entry(int index) const
{
    return entries[index];
}

u8 OAM::readByte(u32 addr)
{
    return data.readByte(addr);
}

u16 OAM::readHalf(u32 addr)
{
    return data.readHalf(addr);
}

u32 OAM::readWord(u32 addr)
{
    return data.readWord(addr);
}

void OAM::writeByte(u32 addr, u8 byte)
{
    EGG_UNSUPPORTED;
}

void OAM::writeHalf(u32 addr, u16 half)
{
    int attr = addr & 0x7;
    if (attr != 0x6)
    {
        // 0x0: Attribute 0
        // 0x2: Attribute 1
        // 0x4: Attribute 2
        // 0x6: Affine parameter (ignored)
        entries[addr >> 3].writeHalf(attr, half);
    }
    data.writeHalf(addr, half);
}

void OAM::writeWord(u32 addr, u32 word)
{
    writeHalf(addr + 0, (word >>  0) & 0xFFFF);
    writeHalf(addr + 2, (word >> 16) & 0xFFFF);
}
