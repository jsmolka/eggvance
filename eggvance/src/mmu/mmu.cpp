#include "mmu.h"

#include <fstream>

#include "common/format.h"
#include "common/utility.h"
#include "map.h"

MMU::MMU()
    : vcount(ref<u8>(REG_VCOUNT))
    , intr_request(ref<u16>(REG_IF))
    , intr_enabled(ref<u16>(REG_IE))
    , timer_data 
      {
          TimerData(ref<u16>(REG_TM0D)),
          TimerData(ref<u16>(REG_TM1D)),
          TimerData(ref<u16>(REG_TM2D)),
          TimerData(ref<u16>(REG_TM3D)) 
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
        break;

    case REG_DISPCNT+1:
        dispcnt.bg0    = bits<0, 1>(byte);
        dispcnt.bg1    = bits<1, 1>(byte);
        dispcnt.bg2    = bits<2, 1>(byte);
        dispcnt.bg3    = bits<3, 1>(byte);
        dispcnt.obj    = bits<4, 1>(byte);
        dispcnt.win0   = bits<5, 1>(byte);
        dispcnt.win1   = bits<6, 1>(byte);
        dispcnt.winobj = bits<7, 1>(byte);
        break;

    case REG_DISPSTAT:
        dispstat.vblank_irq = bits<3, 1>(byte);
        dispstat.hblank_irq = bits<4, 1>(byte);
        dispstat.vcount_irq = bits<5, 1>(byte);
        break;

    case REG_DISPSTAT+1:
        dispstat.vcount_compare = byte;
        break;

    case REG_VCOUNT:
    case REG_VCOUNT+1:
        return;

    case REG_BG0CNT:
        bgcnt[0].priority     = bits<0, 2>(byte);
        bgcnt[0].tile_base    = bits<2, 3>(byte);
        bgcnt[0].mosaic       = bits<6, 1>(byte);
        bgcnt[0].palette_type = bits<7, 1>(byte);
        break;

    case REG_BG0CNT+1:
        bgcnt[0].map_base    = bits<0, 5>(byte);
        bgcnt[0].wraparound  = bits<5, 1>(byte);
        bgcnt[0].screen_size = bits<6, 2>(byte);
        break;

    case REG_BG1CNT:
        bgcnt[1].priority     = bits<0, 2>(byte);
        bgcnt[1].tile_base    = bits<2, 3>(byte);
        bgcnt[1].mosaic       = bits<6, 1>(byte);
        bgcnt[1].palette_type = bits<7, 1>(byte);
        break;

    case REG_BG1CNT+1:
        bgcnt[1].map_base    = bits<0, 5>(byte);
        bgcnt[1].wraparound  = bits<5, 1>(byte);
        bgcnt[1].screen_size = bits<6, 2>(byte);
        break;

    case REG_BG2CNT:
        bgcnt[2].priority     = bits<0, 2>(byte);
        bgcnt[2].tile_base    = bits<2, 3>(byte);
        bgcnt[2].mosaic       = bits<6, 1>(byte);
        bgcnt[2].palette_type = bits<7, 1>(byte);
        break;

    case REG_BG2CNT+1:
        bgcnt[2].map_base    = bits<0, 5>(byte);
        bgcnt[2].wraparound  = bits<5, 1>(byte);
        bgcnt[2].screen_size = bits<6, 2>(byte);
        break;

    case REG_BG3CNT:
        bgcnt[3].priority     = bits<0, 2>(byte);
        bgcnt[3].tile_base    = bits<2, 3>(byte);
        bgcnt[3].mosaic       = bits<6, 1>(byte);
        bgcnt[3].palette_type = bits<7, 1>(byte);
        break;

    case REG_BG3CNT+1:
        bgcnt[3].map_base    = bits<0, 5>(byte);
        bgcnt[3].wraparound  = bits<5, 1>(byte);
        bgcnt[3].screen_size = bits<6, 2>(byte);
        break;

    case REG_BG0HOFS:
        bghofs[0].bytes[0] = byte;
        break;

    case REG_BG0HOFS+1:
        bghofs[0].bytes[1] = byte & 0x1;
        break;

    case REG_BG0VOFS:
        bgvofs[0].bytes[0] = byte;
        break;

    case REG_BG0VOFS+1:
        bgvofs[0].bytes[1] = byte & 0x1;
        break;

    case REG_BG1HOFS:
        bghofs[1].bytes[0] = byte;
        break;

    case REG_BG1HOFS+1:
        bghofs[1].bytes[1] = byte & 0x1;
        break;

    case REG_BG1VOFS:
        bgvofs[1].bytes[0] = byte;
        break;

    case REG_BG1VOFS+1:
        bgvofs[1].bytes[1] = byte & 0x1;
        break;

    case REG_BG2HOFS:
        bghofs[2].bytes[0] = byte;
        break;

    case REG_BG2HOFS+1:
        bghofs[2].bytes[1] = byte & 0x1;
        break;

    case REG_BG2VOFS:
        bgvofs[2].bytes[0] = byte;
        break;

    case REG_BG2VOFS+1:
        bgvofs[2].bytes[1] = byte & 0x1;
        break;

    case REG_BG3HOFS:
        bghofs[3].bytes[0] = byte;
        break;

    case REG_BG3HOFS+1:
        bghofs[3].bytes[1] = byte & 0x1;
        break;

    case REG_BG3VOFS:
        bgvofs[3].bytes[0] = byte;
        break;

    case REG_BG3VOFS+1:
        bgvofs[3].bytes[1] = byte & 0x1;
        break;

    case REG_BG2X:
        bgx[0].bytes[0] = byte;
        bgx[0].internal = bgx[0].value;
        break;

    case REG_BG2X+1:
        bgx[0].bytes[1] = byte;
        bgx[0].internal = bgx[0].value;
        break;

    case REG_BG2X+2:
        bgx[0].bytes[2] = byte;
        bgx[0].internal = bgx[0].value;
        break;

    case REG_BG2X+3:
        bgx[0].bytes[3] = signExtend<4>(byte);
        bgx[0].internal = bgx[0].value;
        break;

    case REG_BG3X:
        bgx[1].bytes[0] = byte;
        bgx[1].internal = bgx[1].value;
        break;

    case REG_BG3X+1:
        bgx[1].bytes[1] = byte;
        bgx[1].internal = bgx[1].value;
        break;

    case REG_BG3X+2:
        bgx[1].bytes[2] = byte;
        bgx[1].internal = bgx[1].value;
        break;

    case REG_BG3X+3:
        bgx[1].bytes[3] = signExtend<4>(byte);
        bgx[1].internal = bgx[1].value;
        break;

    case REG_BG2Y:
        bgy[0].bytes[0] = byte;
        bgy[0].internal = bgy[0].value;
        break;

    case REG_BG2Y+1:
        bgy[0].bytes[1] = byte;
        bgy[0].internal = bgy[0].value;
        break;

    case REG_BG2Y+2:
        bgy[0].bytes[2] = byte;
        bgy[0].internal = bgy[0].value;
        break;

    case REG_BG2Y+3:
        bgy[0].bytes[3] = signExtend<4>(byte);
        bgy[0].internal = bgy[0].value;
        break;

    case REG_BG3Y:
        bgy[1].bytes[0] = byte;
        bgy[1].internal = bgy[1].value;
        break;

    case REG_BG3Y+1:
        bgy[1].bytes[1] = byte;
        bgy[1].internal = bgy[1].value;
        break;

    case REG_BG3Y+2:
        bgy[1].bytes[2] = byte;
        bgy[1].internal = bgy[1].value;
        break;

    case REG_BG3Y+3:
        bgy[1].bytes[3] = signExtend<4>(byte);
        bgy[1].internal = bgy[1].value;
        break;

    case REG_BG2PA:
        bgpa[0].bytes[0] = byte;
        break;

    case REG_BG2PA+1:
        bgpa[0].bytes[1] = byte;
        break;

    case REG_BG2PB:
        bgpb[0].bytes[0] = byte;
        break;

    case REG_BG2PB+1:
        bgpb[0].bytes[1] = byte;
        break;

    case REG_BG2PC:
        bgpc[0].bytes[0] = byte;
        break;

    case REG_BG2PC+1:
        bgpc[0].bytes[1] = byte;
        break;

    case REG_BG2PD:
        bgpd[0].bytes[0] = byte;
        break;

    case REG_BG2PD+1:
        bgpd[0].bytes[1] = byte;
        break;

    case REG_BG3PA:
        bgpa[1].bytes[0] = byte;
        break;

    case REG_BG3PA+1:
        bgpa[1].bytes[1] = byte;
        break;

    case REG_BG3PB:
        bgpb[1].bytes[0] = byte;
        break;

    case REG_BG3PB+1:
        bgpb[1].bytes[1] = byte;
        break;

    case REG_BG3PC:
        bgpc[1].bytes[0] = byte;
        break;

    case REG_BG3PC+1:
        bgpc[1].bytes[1] = byte;
        break;

    case REG_BG3PD:
        bgpd[1].bytes[0] = byte;
        break;

    case REG_BG3PD+1:
        bgpd[1].bytes[1] = byte;
        break;

    case REG_WIN0H:
        winh[0].max = byte;
        break;

    case REG_WIN0H+1:
        winh[0].min = byte;
        break;

    case REG_WIN1H:
        winh[1].max = byte;
        break;

    case REG_WIN1H+1:
        winh[1].min = byte;
        break;

    case REG_WIN0V:
        winv[0].max = byte;
        break;

    case REG_WIN0V+1:
        winv[0].min = byte;
        break;

    case REG_WIN1V:
        winv[1].max = byte;
        break;

    case REG_WIN1V+1:
        winv[1].min = byte;
        break;

    case REG_WININ:
        winin.win0.bg0 = bits<0, 1>(byte);
        winin.win0.bg1 = bits<1, 1>(byte);
        winin.win0.bg2 = bits<2, 1>(byte);
        winin.win0.bg3 = bits<3, 1>(byte);
        winin.win0.obj = bits<4, 1>(byte);
        winin.win0.sfx = bits<5, 1>(byte);
        break;

    case REG_WININ+1:
        winin.win1.bg0 = bits<0, 1>(byte);
        winin.win1.bg1 = bits<1, 1>(byte);
        winin.win1.bg2 = bits<2, 1>(byte);
        winin.win1.bg3 = bits<3, 1>(byte);
        winin.win1.obj = bits<4, 1>(byte);
        winin.win1.sfx = bits<5, 1>(byte);
        break;
        
    case REG_WINOUT:
        winout.winout.bg0 = bits<0, 1>(byte);
        winout.winout.bg1 = bits<1, 1>(byte);
        winout.winout.bg2 = bits<2, 1>(byte);
        winout.winout.bg3 = bits<3, 1>(byte);
        winout.winout.obj = bits<4, 1>(byte);
        winout.winout.sfx = bits<5, 1>(byte);
        break;

    case REG_WINOUT+1:
        winout.winobj.bg0 = bits<0, 1>(byte);
        winout.winobj.bg1 = bits<1, 1>(byte);
        winout.winobj.bg2 = bits<2, 1>(byte);
        winout.winobj.bg3 = bits<3, 1>(byte);
        winout.winobj.obj = bits<4, 1>(byte);
        winout.winobj.sfx = bits<5, 1>(byte);
        break;

    case REG_MOSAIC:
        mosaic.bg_x  = bits< 0, 4>(byte);
        mosaic.bg_y  = bits< 4, 4>(byte);
        mosaic.obj_x = bits< 8, 4>(byte);
        mosaic.obj_y = bits<12, 4>(byte);
        break;

    case REG_BLDCNT:
        bldcnt.upper.bg0 = bits<0, 1>(byte);
        bldcnt.upper.bg1 = bits<1, 1>(byte);
        bldcnt.upper.bg2 = bits<2, 1>(byte);
        bldcnt.upper.bg3 = bits<3, 1>(byte);
        bldcnt.upper.obj = bits<4, 1>(byte);
        bldcnt.upper.bdp = bits<5, 1>(byte);
        bldcnt.mode  = bits<6, 2>(byte);
        break;

    case REG_BLDCNT+1:
        bldcnt.lower.bg0 = bits<0, 1>(byte);
        bldcnt.lower.bg1 = bits<1, 1>(byte);
        bldcnt.lower.bg2 = bits<2, 1>(byte);
        bldcnt.lower.bg3 = bits<3, 1>(byte);
        bldcnt.lower.obj = bits<4, 1>(byte);
        bldcnt.lower.bdp = bits<5, 1>(byte);
        break;

    case REG_BLDALPHA:
        bldalpha.eva = bits<0, 5>(byte);
        break;

    case REG_BLDALPHA+1:
        bldalpha.evb = bits<0, 5>(byte);
        break;

    case REG_BLDY:
        bldy.evy = bits<0, 5>(byte);
        break;

    case REG_WAITCNT:
        waitcnt.sram  = bits<0, 2>(byte);
        waitcnt.ws0.n = bits<2, 2>(byte);
        waitcnt.ws0.s = bits<4, 1>(byte);
        waitcnt.ws1.n = bits<5, 2>(byte);
        waitcnt.ws1.s = bits<7, 1>(byte);
        break;

    case REG_WAITCNT+1:
        waitcnt.ws2.n    = bits<0, 2>(byte);
        waitcnt.ws2.s    = bits<2, 1>(byte);
        waitcnt.phi      = bits<3, 2>(byte);
        waitcnt.prefetch = bits<6, 1>(byte);
        waitcnt.type     = bits<7, 1>(byte);
        break;

    case REG_KEYINPUT:
    case REG_KEYINPUT+1:
        return;

    case REG_TM0D:
        timer[0].initial = (timer[0].initial & ~0x00FF) | byte;
        break;

    case REG_TM0D+1:
        timer[0].initial = (timer[0].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM1D:
        timer[1].initial = (timer[1].initial & ~0x00FF) | byte;
        break;

    case REG_TM1D+1:
        timer[1].initial = (timer[1].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM2D:
        timer[2].initial = (timer[2].initial & ~0x00FF) | byte;
        break;

    case REG_TM2D+1:
        timer[2].initial = (timer[2].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM3D:
        timer[3].initial = (timer[3].initial & ~0x00FF) | byte;
        break;

    case REG_TM3D+1:
        timer[3].initial = (timer[3].initial & ~0xFF00) | (byte << 8);
        break;

    case REG_TM0CNT: 
        if (!timer_control[0].enabled && (byte & 0x80)) 
            timer[0].init();

        timer_control[0].prescaler = bits<0, 2>(byte);
        timer_control[0].cascade   = bits<2, 1>(byte);
        timer_control[0].irq       = bits<6, 1>(byte);
        timer_control[0].enabled   = bits<7, 1>(byte);
        break;
    
    case REG_TM1CNT: 
        if (!timer_control[1].enabled && (byte & 0x80)) 
            timer[1].init(); 

        timer_control[1].prescaler = bits<0, 2>(byte);
        timer_control[1].cascade   = bits<2, 1>(byte);
        timer_control[1].irq       = bits<6, 1>(byte);
        timer_control[1].enabled   = bits<7, 1>(byte);
        break;

    case REG_TM2CNT: 
        if (!timer_control[2].enabled && (byte & 0x80)) 
            timer[2].init(); 

        timer_control[2].prescaler = bits<0, 2>(byte);
        timer_control[2].cascade   = bits<2, 1>(byte);
        timer_control[2].irq       = bits<6, 1>(byte);
        timer_control[2].enabled   = bits<7, 1>(byte);
        break;

    case REG_TM3CNT: 
        if (!timer_control[3].enabled && (byte & 0x80)) 
            timer[3].init();

        timer_control[3].prescaler = bits<0, 2>(byte);
        timer_control[3].cascade   = bits<2, 1>(byte);
        timer_control[3].irq       = bits<6, 1>(byte);
        timer_control[3].enabled   = bits<7, 1>(byte);
        break;

    case REG_KEYCNT:
        keycnt.bytes[0] = byte;
        break;

    case REG_KEYCNT+1:
        keycnt.bytes[1] = byte & 0x3;
        keycnt.irq      = bits<6, 1>(byte);
        keycnt.logic    = bits<7, 1>(byte);
        break;

    case REG_IME:
        intr_master = bits<0, 1>(byte);
        break;

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
