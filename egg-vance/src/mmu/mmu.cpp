#include "mmu.h"

#include <fstream>

#include "common/format.h"
#include "map.h"

MMU::MMU()
    : dispcnt(ref<u16>(REG_DISPCNT))
    , dispstat(ref<u16>(REG_DISPSTAT))
    , vcount(ref<u16>(REG_VCOUNT))
    , bg0cnt(ref<u16>(REG_BG0CNT))
    , bg1cnt(ref<u16>(REG_BG1CNT))
    , bg2cnt(ref<u16>(REG_BG2CNT))
    , bg3cnt(ref<u16>(REG_BG3CNT))
    , keycnt(ref<u16>(REG_KEYCNT))
    , keyinput(ref<u16>(REG_KEYINPUT))
    , waitcnt(ref<u16>(REG_WAITCNT))
{

}

void MMU::reset()
{
    memory.fill(0);
}

bool MMU::readFile(const std::string& file, u32 addr)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
    {
        fmt::printf("Cannot open file {}\n", file);
        return false;
    }

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    u8* memory_ptr = &memory[addr];
    stream.read(reinterpret_cast<char*>(memory_ptr), size);

    return true;
}

u8 MMU::readByte(u32 addr) const
{
    switch ((addr >> 30) & 0x1)
    {
    // Waitstate 1
    case 0xA:
        addr -= 0x2000000;
        break;

    // Waitstate 2
    case 0xC:
        addr -= 0x4000000;
        break;
    }

    return memory[addr];
}

u16 MMU::readHalf(u32 addr) const
{
    return (readByte(addr + 1) << 8) | readByte(addr);
}

u32 MMU::readWord(u32 addr) const
{
    return (readHalf(addr + 2) << 16) | readHalf(addr);
}

u8 MMU::readByteFast(u32 addr)
{
    return memory[addr];
}

u16 MMU::readHalfFast(u32 addr)
{
    return ref<u16>(addr);
}

u32 MMU::readWordFast(u32 addr)
{
    return ref<u32>(addr);
}

void MMU::writeByte(u32 addr, u8 byte)
{
    memory[addr] = byte;
}

void MMU::writeHalf(u32 addr, u16 half)
{
    ref<u16>(addr) = half;
}

void MMU::writeWord(u32 addr, u32 word)
{
    ref<u32>(addr) = word;
}

template<typename T>
T& MMU::ref(u32 addr)
{
    return *reinterpret_cast<T*>(&memory[addr]);
}
