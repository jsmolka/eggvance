#include "mmu.h"

#include <iostream>
#include <fstream>

#include "common/memory_map.h"

MMU::MMU()
    : dispcnt(registerData(REG_DISPCNT))
    , dispstat(registerData(REG_DISPSTAT))
    , vcount(registerData(REG_VCOUNT))
    , bg0cnt(registerData(REG_BG0CNT))
    , bg1cnt(registerData(REG_BG1CNT))
    , bg2cnt(registerData(REG_BG2CNT))
    , bg3cnt(registerData(REG_BG3CNT))
    , keyinput(registerData(REG_KEYINPUT))
    , waitcnt(registerData(REG_WAITCNT))
{

}

void MMU::reset()
{
    std::fill(memory.begin(), memory.end(), 0);
}

bool MMU::loadRom(const std::string& file)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
    {
        std::cout << "Cannot open file " << file << "\n";
        return false;
    }

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    u8* memory_ptr = &memory[MAP_GAMEPAK_0];
    stream.read(reinterpret_cast<char*>(memory_ptr), size);

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
    writeByte(addr + 0, (half >> 0) & 0xFF);
    writeByte(addr + 1, (half >> 8) & 0xFF);
}

void MMU::writeWord(u32 addr, u32 word)
{
    writeByte(addr + 0, (word >>  0) & 0xFF);
    writeByte(addr + 1, (word >>  8) & 0xFF);
    writeByte(addr + 2, (word >> 16) & 0xFF);
    writeByte(addr + 3, (word >> 24) & 0xFF);
}

u16& MMU::registerData(u32 addr)
{
    return *reinterpret_cast<u16*>(&memory[addr]);
}
