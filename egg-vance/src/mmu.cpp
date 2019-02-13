#include "mmu.h"

#include <fstream>

#include "common.h"
#include "memory_map.h"

Mmu::Mmu()
    : lcd_stat(nullptr)
{
    memory.resize(0x10000000, 0);
}

void Mmu::reset()
{
    std::fill(memory.begin(), memory.end(), 0);
}

bool Mmu::loadRom(const std::string& file)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
    {
        fcout() << "Cannot open file " << file;
        return false;
    }

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    u8* memory_ptr = &memory[MAP_GAMEPAK_0];
    stream.read(reinterpret_cast<char*>(memory_ptr), size);

    return true;
}

u8 Mmu::readByte(u32 addr) const
{
    return memory[addr];
}

u16 Mmu::readHalf(u32 addr) const
{
    return (readByte(addr + 1) << 8) | readByte(addr);
}

u32 Mmu::readWord(u32 addr) const
{
    return (readByte(addr + 3) << 24) | (readByte(addr + 2) << 16) | (readByte(addr + 1) << 8) | readByte(addr);
}

void Mmu::writeByte(u32 addr, u8 byte)
{
    memory[addr] = byte;

    switch (addr)
    {
    case REG_DISPCNT:
    case REG_DISPCNT + 1:
        lcd_stat->display_control = readHalf(REG_DISPCNT);
        break;

    case REG_DISPSTAT:
    case REG_DISPSTAT + 1:
        lcd_stat->display_stat = readHalf(REG_DISPSTAT);
        break;

    case REG_VCOUNT:
    case REG_VCOUNT + 1:
        lcd_stat->vcount = readHalf(REG_VCOUNT);
        break;

    case REG_BG0CNT:
    case REG_BG0CNT + 1:
    case REG_BG1CNT:
    case REG_BG1CNT + 1:
    case REG_BG2CNT:
    case REG_BG2CNT + 1:
    case REG_BG3CNT:
    case REG_BG3CNT + 1:
    {
        u8 index = 0;
        switch (addr)
        {
        case REG_BG0CNT: case REG_BG0CNT + 1: index = 0; break;
        case REG_BG1CNT: case REG_BG1CNT + 1: index = 1; break;
        case REG_BG2CNT: case REG_BG2CNT + 1: index = 2; break;
        case REG_BG3CNT: case REG_BG3CNT + 1: index = 3; break;
        }

        u16 half = readHalf(REG_BG0CNT + 2 * index);

        lcd_stat->bg_priority[index]     = half & 0x3;
        lcd_stat->bg_data_addr[index]    = MAP_VRAM + 0x4000 * (half >> 2 & 0x3);
        lcd_stat->bg_mosaic[index]       = half >> 6 & 0x1;
        lcd_stat->bg_palette_type[index] = half >> 7 & 0x1;
        lcd_stat->bg_map_addr[index]     = MAP_VRAM + 0x800 * (half >> 8 & 0xF);
        lcd_stat->bg_map_size[index]     = half >> 14 & 0x3;

        break;
    }
    }
}

void Mmu::writeHalf(u32 addr, u16 half)
{
    writeByte(addr, half & 0xFF);
    writeByte(addr + 1, half >> 8 & 0xFF);
}

void Mmu::writeWord(u32 addr, u32 word)
{
    writeByte(addr, word & 0xFF);
    writeByte(addr + 1, word >> 8 & 0xFF);
    writeByte(addr + 2, word >> 16 & 0xFF);
    writeByte(addr + 3, word >> 24 & 0xFF);
}
