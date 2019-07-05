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
    , bg0hofs(ref<u16>(REG_BG0HOFS))
    , bg1hofs(ref<u16>(REG_BG1HOFS))
    , bg2hofs(ref<u16>(REG_BG2HOFS))
    , bg3hofs(ref<u16>(REG_BG3HOFS))
    , bg0vofs(ref<u16>(REG_BG0VOFS))
    , bg1vofs(ref<u16>(REG_BG1VOFS))
    , bg2vofs(ref<u16>(REG_BG2VOFS))
    , bg3vofs(ref<u16>(REG_BG3VOFS))
    , bg2x(ref<u32>(REG_BG2X))
    , bg3x(ref<u32>(REG_BG3X))
    , bg2y(ref<u32>(REG_BG2Y))
    , bg3y(ref<u32>(REG_BG3Y))
    , bg2pa(ref<u16>(REG_BG2PA))
    , bg3pa(ref<u16>(REG_BG3PA))
    , bg2pb(ref<u16>(REG_BG2PB))
    , bg3pb(ref<u16>(REG_BG3PB))
    , bg2pc(ref<u16>(REG_BG2PC))
    , bg3pc(ref<u16>(REG_BG3PC))
    , bg2pd(ref<u16>(REG_BG2PD))
    , bg3pd(ref<u16>(REG_BG3PD))
    , mosaic(ref<u16>(REG_MOSAIC))
    , bldcnt(ref<u16>(REG_BLDCNT))
    , bldalpha(ref<u16>(REG_BLDALPHA))
    , bldy(ref<u16>(REG_BLDY))
    , win0h(ref<u16>(REG_WIN0H))
    , win1h(ref<u16>(REG_WIN1H))
    , win0v(ref<u16>(REG_WIN0V))
    , win1v(ref<u16>(REG_WIN1V))
    , winin(ref<u16>(REG_WININ))
    , winout(ref<u16>(REG_WINOUT))
    , keycnt(ref<u16>(REG_KEYCNT))
    , keyinput(ref<u16>(REG_KEYINPUT))
    , waitcnt(ref<u16>(REG_WAITCNT))
    , int_master(ref<u32>(REG_IME))
    , int_enabled(ref<u16>(REG_IE))
    , int_request(ref<u16>(REG_IF))
{

}

void MMU::reset()
{
    memory.fill(0);

    halt = false;
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

void MMU::dump(u32 start, u32 size)
{
    std::ofstream file("dump.bin");
    for (u32 i = start; i < (start + size); ++i)
    {
        file << memory[i];
    }
}

void MMU::requestInterrupt(InterruptFlag flag)
{
    if (int_master.enabled)
    {
        if (int_enabled & flag)
            halt = false;

        int_request = int_request | flag;
    }
}

u8 MMU::readByte(u32 addr) const
{
    switch (addr)
    {
    case REG_BG0HOFS:
    case REG_BG0HOFS+1:
    case REG_BG0VOFS:
    case REG_BG0VOFS+1:
    case REG_BG1HOFS:
    case REG_BG1HOFS+1:
    case REG_BG1VOFS:
    case REG_BG1VOFS+1:
    case REG_BG2HOFS:
    case REG_BG2HOFS+1:
    case REG_BG2VOFS:
    case REG_BG2VOFS+1:
    case REG_BG3HOFS:
    case REG_BG3HOFS+1:
    case REG_BG3VOFS:
    case REG_BG3VOFS+1:
    case REG_BG2X:
    case REG_BG2X+1:
    case REG_BG2X+2:
    case REG_BG2X+3:
    case REG_BG2Y:
    case REG_BG2Y+1:
    case REG_BG2Y+2:
    case REG_BG2Y+3:
    case REG_BG3X:
    case REG_BG3X+1:
    case REG_BG3X+2:
    case REG_BG3X+3:
    case REG_BG3Y:
    case REG_BG3Y+1:
    case REG_BG3Y+2:
    case REG_BG3Y+3:
    case REG_BG2PA:
    case REG_BG2PA+1:
    case REG_BG2PB:
    case REG_BG2PB+1:
    case REG_BG2PC:
    case REG_BG2PC+1:
    case REG_BG2PD:
    case REG_BG2PD+1:
    case REG_BG3PA:
    case REG_BG3PA+1:
    case REG_BG3PB:
    case REG_BG3PB+1:
    case REG_BG3PC:
    case REG_BG3PC+1:
    case REG_BG3PD:
    case REG_BG3PD+1:
    case REG_WIN0H:
    case REG_WIN0H+1:
    case REG_WIN1H:
    case REG_WIN1H+1:
    case REG_WIN0V:
    case REG_WIN0V+1:
    case REG_WIN1V:
    case REG_WIN1V+1:
    case REG_MOSAIC:
    case REG_MOSAIC+1:
    case REG_MOSAIC+2:
    case REG_MOSAIC+3:
    case REG_BLDALPHA:
    case REG_BLDALPHA+1:
    case REG_BLDY:
    case REG_BLDY+1:
    case REG_BLDY+2:
    case REG_BLDY+3:
        return 0;
    }

    switch ((addr >> 30) & 0xF)
    {
    // Waitstate 1
    case 0xA:
    case 0xB:
        addr -= 0x2000000;
        break;

    // Waitstate 2
    case 0xC:
    case 0xD:
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
    switch (addr)
    {
    case REG_VCOUNT:
    case REG_VCOUNT+1:
    case REG_KEYINPUT:
    case REG_KEYINPUT+1:
        return;

    // Protect status flags
    case REG_DISPSTAT:
        byte &= ~0x7;
        break;

    // Acknowledge interrupt, writing clears
    case REG_IF:
    case REG_IF+1:
        memory[addr] &= ~byte;
        return;
        
    case REG_HALTCNT:
        halt = true;
        break;
    }
    memory[addr] = byte;
}

void MMU::writeHalf(u32 addr, u16 half)
{
    writeByte(addr, half & 0xFF);
    writeByte(addr + 1, half >> 8);
}

void MMU::writeWord(u32 addr, u32 word)
{
    writeHalf(addr, word & 0xFFFF);
    writeHalf(addr + 2, word >> 16);
}

template<typename T>
T& MMU::ref(u32 addr)
{
    return *reinterpret_cast<T*>(&memory[addr]);
}
