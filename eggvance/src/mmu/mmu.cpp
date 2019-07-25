#include "mmu.h"

#include <fstream>

#include "common/format.h"
#include "common/utility.h"
#include "map.h"

MMU::MMU()
    : dispstat(ref<u16>(REG_DISPSTAT))
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
    , timer_data 
      {
          TimerData(ref<u16>(REG_TM0D)),
          TimerData(ref<u16>(REG_TM1D)),
          TimerData(ref<u16>(REG_TM2D)),
          TimerData(ref<u16>(REG_TM3D)) 
      }
    , timer_control 
      {
          TimerControl(ref<u16>(REG_TM0CNT)),
          TimerControl(ref<u16>(REG_TM1CNT)),
          TimerControl(ref<u16>(REG_TM2CNT)),
          TimerControl(ref<u16>(REG_TM3CNT)) 
      }
    , timer 
      { 
          Timer(timer_control[0], timer_data[0]), 
          Timer(timer_control[1], timer_data[1]), 
          Timer(timer_control[2], timer_data[2]), 
          Timer(timer_control[3], timer_data[3])
      }
{
    timer[0].next = &timer[1];
    timer[1].next = &timer[2];
    timer[2].next = &timer[3];

    timer[1].prev = &timer[0];
    timer[2].prev = &timer[1];
    timer[3].prev = &timer[2];

    // Todo: how are registers initialized when not using the bios?
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
        fmt::print("Cannot open file {}\n", file);
        return false;
    }

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    u8* memory_ptr = &memory[addr];
    stream.read(reinterpret_cast<char*>(memory_ptr), size);

    return true;
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
    demirror(addr);

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
    demirror(addr);

    switch (addr)
    {
    case REG_DISPCNT:
        dispcnt.mode        = bits<0, 3>(byte);
        dispcnt.gbc         = bits<3, 1>(byte);
        dispcnt.frame       = bits<4, 1>(byte);
        dispcnt.oam_hblank  = bits<5, 1>(byte);
        dispcnt.mapping_1d  = bits<6, 1>(byte);
        dispcnt.force_blank = bits<7, 1>(byte);
        dispcnt.bytes[0]    = byte;
        break;

    case REG_DISPCNT+1:
        dispcnt.bg0      = bits<0, 1>(byte);
        dispcnt.bg1      = bits<1, 1>(byte);
        dispcnt.bg2      = bits<2, 1>(byte);
        dispcnt.bg3      = bits<3, 1>(byte);
        dispcnt.obj      = bits<4, 1>(byte);
        dispcnt.win0     = bits<5, 1>(byte);
        dispcnt.win1     = bits<6, 1>(byte);
        dispcnt.winobj   = bits<7, 1>(byte);
        dispcnt.bytes[1] = byte;
        break;

    case REG_VCOUNT:
    case REG_VCOUNT+1:
    case REG_KEYINPUT:
    case REG_KEYINPUT+1:
        return;

    case REG_TM0D:
        timer[0].initial = (timer[0].initial & ~0x00FF) | byte;
        break;

    case REG_TM0D + 1:
        timer[0].initial = (timer[0].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM1D:
        timer[1].initial = (timer[1].initial & ~0x00FF) | byte;
        break;

    case REG_TM1D + 1:
        timer[1].initial = (timer[1].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM2D:
        timer[2].initial = (timer[2].initial & ~0x00FF) | byte;
        break;

    case REG_TM2D + 1:
        timer[2].initial = (timer[2].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM3D:
        timer[3].initial = (timer[3].initial & ~0x00FF) | byte;
        break;

    case REG_TM3D + 1:
        timer[3].initial = (timer[3].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM0CNT: if (!timer_control[0].enabled && (byte & 0x80)) timer[0].init(); break;
    case REG_TM1CNT: if (!timer_control[1].enabled && (byte & 0x80)) timer[1].init(); break;
    case REG_TM2CNT: if (!timer_control[2].enabled && (byte & 0x80)) timer[2].init(); break;
    case REG_TM3CNT: if (!timer_control[3].enabled && (byte & 0x80)) timer[3].init(); break;

        // Protect status flags
    case REG_DISPSTAT:
        byte &= ~0x7;
        break;

        // Acknowledge interrupt, writing clears
    case REG_IF:
    case REG_IF + 1:
        memory[addr] &= ~byte;
        return;

    case REG_HALTCNT:
        halt = true;
        break;
    }

    memory[addr] = byte;

    postWrite(addr);
}

void MMU::writeHalf(u32 addr, u16 half)
{
    writeByte(addr, static_cast<u8>(half));
    writeByte(addr + 1, half >> 8);
}

void MMU::writeWord(u32 addr, u32 word)
{
    writeHalf(addr, static_cast<u16>(word));
    writeHalf(addr + 2, word >> 16);
}

void MMU::writeByteFast(u32 addr, u8 byte)
{
    memory[addr] = byte;
}

void MMU::writeHalfFast(u32 addr, u16 half)
{
    ref<u16>(addr) = half;
}

void MMU::writeWordFast(u32 addr, u32 word)
{
    ref<u32>(addr) = word;
}

template<typename T>
T& MMU::ref(u32 addr)
{
    return *reinterpret_cast<T*>(&memory[addr]);
}

void MMU::postWrite(u32 addr)
{
    switch (addr)
    {
    case REG_BG2X:
    case REG_BG2X+1:
    case REG_BG2X+2:
    case REG_BG2X+3:
        bgx[0].moveToInternal();
        break;

    case REG_BG2Y:
    case REG_BG2Y+1:
    case REG_BG2Y+2:
    case REG_BG2Y+3:
        bgy[0].moveToInternal();
        break;

    case REG_BG3X:
    case REG_BG3X+1:
    case REG_BG3X+2:
    case REG_BG3X+3:
        bgx[1].moveToInternal();
        break;

    case REG_BG3Y:
    case REG_BG3Y+1:
    case REG_BG3Y+2:
    case REG_BG3Y+3:
        bgy[1].moveToInternal();
        break;
    }
}

void MMU::demirror(u32& addr) const
{
    switch (addr >> 24)
    {
    // Todo: should BIOS memory be mirrored?
    case 0x0:
    case 0x1:
    case 0x4:
    case 0x8:
    case 0x9:
        break;

    // On-board work RAM
    case 0x2:
        addr &= 0x0203FFFF;
        break;

    // On-chip work RAM
    case 0x3:
        addr &= 0x03007FFF;
        break;

    // Palette RAM
    case 0x5:
        addr &= 0x050003FF;
        break;

    // VRAM
    case 0x6:
        // Todo: proper VRAM mirroring
        break;

    // OAM
    case 0x7:
        addr &= 0x070003FF;
        break;

    // Waitstate 1
    case 0xA:
    case 0xB:
        addr -= 0x02000000;
        break;
        
    // Waitstate 2
    case 0xC:
    case 0xD:
        addr -= 0x04000000;
        break;

    case 0xE:
    case 0xF:
        // Todo: SRAM
        break;
    }
}
