#include "mmu.h"

#include <fstream>
#include <iterator>

#define GAMEPAK_0 0x8000000

MMU::MMU()
{

}

void MMU::reset()
{
    memory.clear();
    memory.resize(0x10000000, 0);
}

bool MMU::loadRom(const std::string& filepath)
{
    std::ifstream stream(filepath, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.unsetf(std::ios::skipws);

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    std::copy(std::istream_iterator<u8>(stream), std::istream_iterator<u8>(), memory.begin() + GAMEPAK_0);

    return true;
}

u8 MMU::readByte(u32 addr) const
{
    return memory[addr];
}

u16 MMU::readHalf(u32 addr) const
{
    return (readByte(addr + 1) << 8) | readByte(addr);
}

u32 MMU::readWord(u32 addr) const
{
    return (readByte(addr + 3) << 24) | (readByte(addr + 2) << 16) | (readByte(addr + 1) << 8) | readByte(addr);
}

void MMU::writeByte(u32 addr, u8 byte)
{
    memory[addr] = byte;
}

void MMU::writeHalf(u32 addr, u16 half)
{
    writeByte(addr, half & 0xFF);
    writeByte(addr + 1, half >> 8);
}

void MMU::writeWord(u32 addr, u32 word)
{
    writeByte(addr, word & 0xFF);
    writeByte(addr + 1, (word >> 8) & 0xFF);
    writeByte(addr + 2, (word >> 16) & 0xFF);
    writeByte(addr + 3, (word >> 24) & 0xFF);
}
