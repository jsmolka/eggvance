#include "mmu.h"

#include <iostream>
#include <fstream>

#include "common/memory_map.h"

MMU::MMU()
{
    memory.resize(0x10000000, 0);
}

void MMU::reset()
{
    std::fill(memory.begin(), memory.end(), 0);

    dispcnt = { };
    dispstat = { };
    *bgcnt = { };
}

void MMU::dump(u32 start, u32 size)
{
    std::ofstream file("dump.bin");
    for (u32 i = start; i < (start + size); ++i)
    {
        file << memory[i];
    }
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

    switch (addr)
    {
    case REG_DISPCNT: 
        dispcnt.write(0, byte); 
        break;

    case REG_DISPCNT + 1: 
        dispcnt.write(1, byte); 
        break;

    case REG_DISPSTAT: 
        dispstat.write(0, byte); 
        break;

    case REG_DISPSTAT + 1: 
        dispstat.write(1, byte); 
        break; 

    case REG_BG0CNT: 
        bgcnt[0].write(0, byte); 
        break;

    case REG_BG0CNT + 1: 
        bgcnt[0].write(1, byte); 
        break;

    case REG_BG1CNT: 
        bgcnt[1].write(0, byte); 
        break;

    case REG_BG1CNT + 1: 
        bgcnt[1].write(1, byte); 
        break;

    case REG_BG2CNT: 
        bgcnt[2].write(0, byte); 
        break;

    case REG_BG2CNT + 1: 
        bgcnt[2].write(1, byte); 
        break;

    case REG_BG3CNT: 
        bgcnt[3].write(0, byte); 
        break;

    case REG_BG3CNT + 1: 
        bgcnt[3].write(1, byte); 
        break;
    }
}

void MMU::writeHalf(u32 addr, u16 half)
{
    writeByte(addr, half & 0xFF);
    writeByte(addr + 1, half >> 8 & 0xFF);
}

void MMU::writeWord(u32 addr, u32 word)
{
    writeByte(addr, word & 0xFF);
    writeByte(addr + 1, word >> 8 & 0xFF);
    writeByte(addr + 2, word >> 16 & 0xFF);
    writeByte(addr + 3, word >> 24 & 0xFF);
}
