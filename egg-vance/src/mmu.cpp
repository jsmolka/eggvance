#include "mmu.h"

MMU::MMU()
{

}

void MMU::reset()
{
    memory_map.clear();
    memory_map.resize(0x10000000, 0);
}

u8 MMU::read8(u32 address) const
{
    return memory_map[address];
}

u16 MMU::read16(u32 address) const
{
    return (read8(address + 1) << 8) | read8(address);
}

u32 MMU::read32(u32 address) const
{
    // Todo: Use read16
    return (read8(address + 3) << 24) | (read8(address + 2) << 16) | (read8(address + 1) << 8) | read8(address);
}

void MMU::write8(u32 address, u8 value)
{
    memory_map[address] = value;
}

void MMU::write16(u32 address, u16 value)
{
    write8(address, value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
}

void MMU::write32(u32 address, u32 value)
{
    // Todo: Use write16
    write8(address, value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
    write8(address + 2, (value >> 16) & 0xFF);
    write8(address + 3, (value >> 24) & 0xFF);
}
