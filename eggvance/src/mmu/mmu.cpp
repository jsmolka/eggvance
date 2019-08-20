#include "mmu.h"

#include "common/format.h"
#include "common/utility.h"

MMU::MMU(std::unique_ptr<BIOS> bios)
    : bios(std::move(bios))
    , vcount(io.ref<u8>(REG_VCOUNT))
    , intr_request(io.ref<u16>(REG_IF))
    , intr_enabled(io.ref<u16>(REG_IE))
    , keyinput(io.ref<u16>(REG_KEYINPUT))
    , timers{ 
        Timer(0), 
        Timer(1), 
        Timer(2), 
        Timer(3) }
    , dmas{ 
        DMA(0, *this), 
        DMA(1, *this), 
        DMA(2, *this), 
        DMA(3, *this) }
{
    timers[0].next = &timers[1];
    timers[1].next = &timers[2];
    timers[2].next = &timers[3];

    dmas_active.reserve(4);

    reset();
}

void MMU::reset()
{
    ewram.fill(0);
    iwram.fill(0);
    io.fill(0);
    palette.fill(0);
    vram.fill(0);
    oam.fill(0);

    timers[0].reset();
    timers[1].reset();
    timers[2].reset();
    timers[3].reset();

    dmas[0].reset();
    dmas[1].reset();
    dmas[2].reset();
    dmas[3].reset();

    dmas_active.clear();

    for (int i = 0; i < oam.size(); ++i)
    {
        writeByte(MAP_IO + i, 0);
    }
    dispstat.hblank = false;
    dispstat.vblank = false;

    keyinput = 0x3FF;
    halt = false;

    oam_entries.fill(OAMEntry());
}

void MMU::setGamePak(std::unique_ptr<GamePak> gamepak)
{
    this->gamepak = std::move(gamepak);
}

u8 MMU::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        return bios->readByte(addr);

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram[addr];

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram[addr];

    case PAGE_IO:
        return readIO(addr);

    case PAGE_PALETTE:
    case PAGE_VRAM:
    case PAGE_OAM:
        return 0;

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
        addr &= 0x1FF'FFFF;
        return gamepak->readByte(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak->save->type == Save::Type::EEPROM)
        {
            if (gamepak->size() <= 0x1000000 || (addr >= 0xDFF'FF00 && addr < 0xDFF'FFFF))
                return 1;
        }
        addr &= 0x1FF'FFFF;
        return gamepak->readByte(addr);
    
    case PAGE_GAMEPAK_SRAM:
        switch (gamepak->save->type)
        {
        case Save::Type::SRAM:
            addr &= 0x7FFF;
            return gamepak->save->readByte(addr);

        case Save::Type::FLASH64:
        case Save::Type::FLASH128:
            addr &= 0xFFFF;
            return gamepak->save->readByte(addr);
        }
        return 0;

    default:
        if (gamepak->save->type == Save::Type::SRAM)
        {
            addr &= 0x7FFF;
            return gamepak->save->readByte(addr);
        }
        return 0;
    }
    return 0;
}

u16 MMU::readHalf(u32 addr)
{
    addr = alignHalf(addr);
    switch ((addr >> 24) & 0xF)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        return bios->readHalf(addr);

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.get<u16>(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.get<u16>(addr);

    case PAGE_IO:
        return readIO(addr) 
             | readIO(addr + 1) << 8;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        return palette.get<u16>(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        return vram.get<u16>(addr);

    case PAGE_OAM:
        addr &= 0x3FF;
        return oam.get<u16>(addr);

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
        addr &= 0x1FF'FFFF;
        return gamepak->readHalf(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak->save->type == Save::Type::EEPROM)
        {
            if (gamepak->size() <= 0x1000000 || (addr >= 0xDFF'FF00 && addr < 0xDFF'FFFF))
                return 1;
        }
        addr &= 0x1FF'FFFF;
        return gamepak->readHalf(addr);
    }
    return 0;
}

u32 MMU::readWord(u32 addr)
{
    addr = alignWord(addr);
    switch ((addr >> 24) & 0xF)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        return bios->readWord(addr);

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.get<u32>(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.get<u32>(addr);

    case PAGE_IO:
        return readIO(addr) 
             | readIO(addr + 1) << 8
             | readIO(addr + 2) << 16
             | readIO(addr + 3) << 24;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        return palette.get<u32>(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        return vram.get<u32>(addr);

    case PAGE_OAM:
        addr &= 0x3FF;
        return oam.get<u32>(addr);

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
        addr &= 0x1FF'FFFF;
        return gamepak->readWord(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak->save->type == Save::Type::EEPROM)
        {
            if (gamepak->size() <= 0x1000000 || (addr >= 0xDFF'FF00 && addr < 0xDFF'FFFF))
                return 1;
        }
        addr &= 0x1FF'FFFF;
        return gamepak->readWord(addr);
    }
    return 0;
}

void MMU::writeByte(u32 addr, u8 byte)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        ewram[addr] = byte;
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram[addr] = byte;
        break;

    case PAGE_IO:
        writeIO(addr, byte);
        break;

    case PAGE_PALETTE:
    case PAGE_VRAM:
    case PAGE_OAM:
        break;

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
    case PAGE_GAMEPAK_2+1:
        break;

    case PAGE_GAMEPAK_SRAM:
        switch (gamepak->save->type)
        {
        case Save::Type::SRAM:
            addr &= 0x7FFF;
            gamepak->save->writeByte(addr, byte);
            break;

        case Save::Type::FLASH64:
        case Save::Type::FLASH128:
            addr &= 0xFFFF;
            gamepak->save->writeByte(addr, byte);
            break;
        }
        break;

    default:
        if (gamepak->save->type == Save::Type::SRAM)
        {
            addr &= 0x7FFF;
            gamepak->save->writeByte(addr, byte);
        }
        break;
    }
}

void MMU::writeHalf(u32 addr, u16 half)
{
    addr = alignHalf(addr);
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        ewram.ref<u16>(addr) = half;
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram.ref<u16>(addr) = half;
        break;

    case PAGE_IO:
        writeIO(addr + 0, (half >> 0) & 0xFF);
        writeIO(addr + 1, (half >> 8) & 0xFF);
        break;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        palette.ref<u16>(addr) = half;
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        vram.ref<u16>(addr) = half;
        break;

    case PAGE_OAM:
        writeOAM(addr, half);
        break;

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
    case PAGE_GAMEPAK_2+1:
        break;

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        break;
    }
}

void MMU::writeWord(u32 addr, u32 word)
{
    addr = alignWord(addr);
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        ewram.ref<u32>(addr) = word;
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram.ref<u32>(addr) = word;
        break;

    case PAGE_IO:
        writeIO(addr + 0, (word >>  0) & 0xFF);
        writeIO(addr + 1, (word >>  8) & 0xFF);
        writeIO(addr + 2, (word >> 16) & 0xFF);
        writeIO(addr + 3, (word >> 24) & 0xFF);
        break;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        palette.ref<u32>(addr) = word;
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        vram.ref<u32>(addr) = word;
        break;

    case PAGE_OAM:
        writeOAM(addr + 0, (word >>  0) & 0xFFFF);
        writeOAM(addr + 2, (word >> 16) & 0xFFFF);
        break;

    case PAGE_GAMEPAK_0: 
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1: 
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2: 
    case PAGE_GAMEPAK_2+1:
        break;

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        break;
    }
}

void MMU::commitStatus()
{   
    io[REG_DISPSTAT] &= ~0x7;
    io[REG_DISPSTAT] |= (dispstat.vblank << 0);
    io[REG_DISPSTAT] |= (dispstat.hblank << 1);
    io[REG_DISPSTAT] |= (dispstat.vmatch << 2);
}

void MMU::signalDMA(DMA::Timing timing)
{
    bool pushed = false;
    for (DMA& dma : dmas)
    {
        if (!dma.active && dma.control.enable && dma.control.timing == timing)
        {
            dma.activate();
            dmas_active.push_back(&dma);
            pushed = true;
        }
    }
    if (pushed && dmas_active.size() > 1)
    {
        std::sort(dmas_active.begin(), dmas_active.end(), [](const DMA* lhs, const DMA* rhs) {
            return lhs->id > rhs->id;
        });
    }
}

u8 MMU::readIO(u32 addr)
{
    if (addr >= (MAP_IO + 0x400))
        return 0;

    addr &= 0x3FF;
    switch (addr)
    {
    case REG_VCOUNT: 
        return vcount;

    case REG_VCOUNT+1: 
        return 0;

    case REG_BG0HOFS: case REG_BG0HOFS+1:
    case REG_BG0VOFS: case REG_BG0VOFS+1:
    case REG_BG1HOFS: case REG_BG1HOFS+1:
    case REG_BG1VOFS: case REG_BG1VOFS+1:
    case REG_BG2HOFS: case REG_BG2HOFS+1:
    case REG_BG2VOFS: case REG_BG2VOFS+1:
    case REG_BG3HOFS: case REG_BG3HOFS+1:
    case REG_BG3VOFS: case REG_BG3VOFS+1:
        return 0;

    case REG_BG2X: case REG_BG2X+1: case REG_BG2X+2: case REG_BG2X+3:
    case REG_BG2Y: case REG_BG2Y+1: case REG_BG2Y+2: case REG_BG2Y+3:
    case REG_BG3X: case REG_BG3X+1: case REG_BG3X+2: case REG_BG3X+3:
    case REG_BG3Y: case REG_BG3Y+1: case REG_BG3Y+2: case REG_BG3Y+3:
        return 0;

    case REG_BG2PA: case REG_BG2PA+1: 
    case REG_BG2PB: case REG_BG2PB+1: 
    case REG_BG2PC: case REG_BG2PC+1: 
    case REG_BG2PD: case REG_BG2PD+1:
    case REG_BG3PA: case REG_BG3PA+1: 
    case REG_BG3PB: case REG_BG3PB+1:
    case REG_BG3PC: case REG_BG3PC+1: 
    case REG_BG3PD: case REG_BG3PD+1:
        return 0;

    case REG_WIN0H: case REG_WIN0H+1: 
    case REG_WIN1H: case REG_WIN1H+1:
    case REG_WIN0V: case REG_WIN0V+1:
    case REG_WIN1V: case REG_WIN1V+1:
        return 0;

    case REG_MOSAIC: 
    case REG_MOSAIC+1: 
    case REG_MOSAIC+2: 
    case REG_MOSAIC+3:
        return 0;

    case REG_BLDALPHA: 
    case REG_BLDALPHA+1:
        return 0;

    case REG_BLDY:
    case REG_BLDY+1:
    case REG_BLDY+2:
    case REG_BLDY+3:
        return 0;

    case REG_DMA0SAD: case REG_DMA0SAD+1: case REG_DMA0SAD+2: case REG_DMA0SAD+3:
    case REG_DMA1SAD: case REG_DMA1SAD+1: case REG_DMA1SAD+2: case REG_DMA1SAD+3:
    case REG_DMA2SAD: case REG_DMA2SAD+1: case REG_DMA2SAD+2: case REG_DMA2SAD+3:
    case REG_DMA3SAD: case REG_DMA3SAD+1: case REG_DMA3SAD+2: case REG_DMA3SAD+3:
        return 0;

    case REG_DMA0DAD: case REG_DMA0DAD+1: case REG_DMA0DAD+2: case REG_DMA0DAD+3:
    case REG_DMA1DAD: case REG_DMA1DAD+1: case REG_DMA1DAD+2: case REG_DMA1DAD+3:
    case REG_DMA2DAD: case REG_DMA2DAD+1: case REG_DMA2DAD+2: case REG_DMA2DAD+3:
    case REG_DMA3DAD: case REG_DMA3DAD+1: case REG_DMA3DAD+2: case REG_DMA3DAD+3:
        return 0;

    case REG_DMA0CNT_L: case REG_DMA0CNT_L+1:
    case REG_DMA1CNT_L: case REG_DMA1CNT_L+1:
    case REG_DMA2CNT_L: case REG_DMA2CNT_L+1:
    case REG_DMA3CNT_L: case REG_DMA3CNT_L+1:
        return 0;

    case REG_TM0CNT_L+0: return timers[0].data_b[0];
    case REG_TM0CNT_L+1: return timers[0].data_b[1];
    case REG_TM1CNT_L+0: return timers[1].data_b[0];
    case REG_TM1CNT_L+1: return timers[1].data_b[1];
    case REG_TM2CNT_L+0: return timers[2].data_b[0];
    case REG_TM2CNT_L+1: return timers[2].data_b[1];
    case REG_TM3CNT_L+0: return timers[3].data_b[0];
    case REG_TM3CNT_L+1: return timers[3].data_b[1];
    }
    return io[addr];
}

void MMU::writeIO(u32 addr, u8 byte)
{
    if (addr >= (MAP_IO + 0x400))
        return;

    addr &= 0x3FF;
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
        dispstat.vmatch_irq = bits<5, 1>(byte);
        break;

    case REG_DISPSTAT+1:
        dispstat.vcount_compare = byte;
        break;

    case REG_VCOUNT:
    case REG_VCOUNT+1:
        return;

    case REG_BG0CNT+0: writeBackgroundControlLower(bgcnt[0], byte); break;
    case REG_BG0CNT+1: writeBackgroundControlUpper(bgcnt[0], byte); break;
    case REG_BG1CNT+0: writeBackgroundControlLower(bgcnt[1], byte); break;
    case REG_BG1CNT+1: writeBackgroundControlUpper(bgcnt[1], byte); break;
    case REG_BG2CNT+0: writeBackgroundControlLower(bgcnt[2], byte); break;
    case REG_BG2CNT+1: writeBackgroundControlUpper(bgcnt[2], byte); break;
    case REG_BG3CNT+0: writeBackgroundControlLower(bgcnt[3], byte); break;
    case REG_BG3CNT+1: writeBackgroundControlUpper(bgcnt[3], byte); break;

    case REG_BG0HOFS+0: bghofs[0].offset_b[0] = byte; break;
    case REG_BG0HOFS+1: bghofs[0].offset_b[1] = byte & 0x1; break;
    case REG_BG0VOFS+0: bgvofs[0].offset_b[0] = byte; break;
    case REG_BG0VOFS+1: bgvofs[0].offset_b[1] = byte & 0x1; break;
    case REG_BG1HOFS+0: bghofs[1].offset_b[0] = byte; break;
    case REG_BG1HOFS+1: bghofs[1].offset_b[1] = byte & 0x1; break;
    case REG_BG1VOFS+0: bgvofs[1].offset_b[0] = byte; break;
    case REG_BG1VOFS+1: bgvofs[1].offset_b[1] = byte & 0x1; break;
    case REG_BG2HOFS+0: bghofs[2].offset_b[0] = byte; break;
    case REG_BG2HOFS+1: bghofs[2].offset_b[1] = byte & 0x1; break;
    case REG_BG2VOFS+0: bgvofs[2].offset_b[0] = byte; break;
    case REG_BG2VOFS+1: bgvofs[2].offset_b[1] = byte & 0x1; break;
    case REG_BG3HOFS+0: bghofs[3].offset_b[0] = byte; break;
    case REG_BG3HOFS+1: bghofs[3].offset_b[1] = byte & 0x1; break;
    case REG_BG3VOFS+0: bgvofs[3].offset_b[0] = byte; break;
    case REG_BG3VOFS+1: bgvofs[3].offset_b[1] = byte & 0x1; break;

    case REG_BG2X+0: bgx[0].ref_b[0] = byte; bgx[0].internal = bgx[0].ref; break;
    case REG_BG2X+1: bgx[0].ref_b[1] = byte; bgx[0].internal = bgx[0].ref; break;
    case REG_BG2X+2: bgx[0].ref_b[2] = byte; bgx[0].internal = bgx[0].ref; break;
    case REG_BG2X+3: bgx[0].ref_b[3] = signExtend<4>(byte); bgx[0].internal = bgx[0].ref; break;

    case REG_BG3X+0: bgx[1].ref_b[0] = byte; bgx[1].internal = bgx[1].ref; break;
    case REG_BG3X+1: bgx[1].ref_b[1] = byte; bgx[1].internal = bgx[1].ref; break;
    case REG_BG3X+2: bgx[1].ref_b[2] = byte; bgx[1].internal = bgx[1].ref; break;
    case REG_BG3X+3: bgx[1].ref_b[3] = signExtend<4>(byte); bgx[1].internal = bgx[1].ref; break;

    case REG_BG2Y+0: bgy[0].ref_b[0] = byte; bgy[0].internal = bgy[0].ref; break; 
    case REG_BG2Y+1: bgy[0].ref_b[1] = byte; bgy[0].internal = bgy[0].ref; break;
    case REG_BG2Y+2: bgy[0].ref_b[2] = byte; bgy[0].internal = bgy[0].ref; break;
    case REG_BG2Y+3: bgy[0].ref_b[3] = signExtend<4>(byte); bgy[0].internal = bgy[0].ref; break;

    case REG_BG3Y+0: bgy[1].ref_b[0] = byte; bgy[1].internal = bgy[1].ref; break;
    case REG_BG3Y+1: bgy[1].ref_b[1] = byte; bgy[1].internal = bgy[1].ref; break;
    case REG_BG3Y+2: bgy[1].ref_b[2] = byte; bgy[1].internal = bgy[1].ref; break;
    case REG_BG3Y+3: bgy[1].ref_b[3] = signExtend<4>(byte); bgy[1].internal = bgy[1].ref; break;

    case REG_BG2PA+0: bgpa[0].param_b[0] = byte; break;
    case REG_BG2PA+1: bgpa[0].param_b[1] = byte; break;
    case REG_BG2PB+0: bgpb[0].param_b[0] = byte; break;
    case REG_BG2PB+1: bgpb[0].param_b[1] = byte; break;
    case REG_BG2PC+0: bgpc[0].param_b[0] = byte; break;
    case REG_BG2PC+1: bgpc[0].param_b[1] = byte; break;
    case REG_BG2PD+0: bgpd[0].param_b[0] = byte; break;
    case REG_BG2PD+1: bgpd[0].param_b[1] = byte; break;

    case REG_BG3PA+0: bgpa[1].param_b[0] = byte; break;
    case REG_BG3PA+1: bgpa[1].param_b[1] = byte; break;
    case REG_BG3PB+0: bgpb[1].param_b[0] = byte; break;
    case REG_BG3PB+1: bgpb[1].param_b[1] = byte; break;
    case REG_BG3PC+0: bgpc[1].param_b[0] = byte; break;
    case REG_BG3PC+1: bgpc[1].param_b[1] = byte; break;
    case REG_BG3PD+0: bgpd[1].param_b[0] = byte; break;
    case REG_BG3PD+1: bgpd[1].param_b[1] = byte; break;

    case REG_WIN0H+0: winh[0].max = byte; break;
    case REG_WIN0H+1: winh[0].min = byte; break;
    case REG_WIN1H+0: winh[1].max = byte; break;
    case REG_WIN1H+1: winh[1].min = byte; break;

    case REG_WIN0V+0: winv[0].max = byte; break;
    case REG_WIN0V+1: winv[0].min = byte; break;
    case REG_WIN1V+0: winv[1].max = byte; break;
    case REG_WIN1V+1: winv[1].min = byte; break;

    case REG_WININ+0: writeWindow(winin.win0, byte); break;
    case REG_WININ+1: writeWindow(winin.win1, byte); break;

    case REG_WINOUT+0: writeWindow(winout.winout, byte); break;
    case REG_WINOUT+1: writeWindow(winout.winobj, byte); break;

    case REG_MOSAIC:
        mosaic.bg.x  = bits<0, 4>(byte);
        mosaic.bg.y  = bits<4, 4>(byte);
        break;

    case REG_MOSAIC+1:
        mosaic.obj.x = bits<0, 4>(byte);
        mosaic.obj.y = bits<4, 4>(byte);
        break;

    case REG_BLDCNT:
        writeBlendLayer(bldcnt.upper, byte);
        bldcnt.mode = bits<6, 2>(byte);
        break;

    case REG_BLDCNT+1:
        writeBlendLayer(bldcnt.lower, byte);
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

    case REG_DMA0SAD+0: dmas[0].src.addr_b[0] = byte; break;
    case REG_DMA0SAD+1: dmas[0].src.addr_b[1] = byte; break;
    case REG_DMA0SAD+2: dmas[0].src.addr_b[2] = byte; break;
    case REG_DMA0SAD+3: dmas[0].src.addr_b[3] = byte & 0x7; break;
    case REG_DMA1SAD+0: dmas[1].src.addr_b[0] = byte; break;
    case REG_DMA1SAD+1: dmas[1].src.addr_b[1] = byte; break;
    case REG_DMA1SAD+2: dmas[1].src.addr_b[2] = byte; break;
    case REG_DMA1SAD+3: dmas[1].src.addr_b[3] = byte & 0xF; break;
    case REG_DMA2SAD+0: dmas[2].src.addr_b[0] = byte; break;
    case REG_DMA2SAD+1: dmas[2].src.addr_b[1] = byte; break;
    case REG_DMA2SAD+2: dmas[2].src.addr_b[2] = byte; break;
    case REG_DMA2SAD+3: dmas[2].src.addr_b[3] = byte & 0xF; break;
    case REG_DMA3SAD+0: dmas[3].src.addr_b[0] = byte; break;
    case REG_DMA3SAD+1: dmas[3].src.addr_b[1] = byte; break;
    case REG_DMA3SAD+2: dmas[3].src.addr_b[2] = byte; break;
    case REG_DMA3SAD+3: dmas[3].src.addr_b[3] = byte & 0xF; break;

    case REG_DMA0DAD+0: dmas[0].dst.addr_b[0] = byte; break;
    case REG_DMA0DAD+1: dmas[0].dst.addr_b[1] = byte; break;
    case REG_DMA0DAD+2: dmas[0].dst.addr_b[2] = byte; break;
    case REG_DMA0DAD+3: dmas[0].dst.addr_b[3] = byte & 0x7; break;
    case REG_DMA1DAD+0: dmas[1].dst.addr_b[0] = byte; break;
    case REG_DMA1DAD+1: dmas[1].dst.addr_b[1] = byte; break;
    case REG_DMA1DAD+2: dmas[1].dst.addr_b[2] = byte; break;
    case REG_DMA1DAD+3: dmas[1].dst.addr_b[3] = byte & 0x7; break;
    case REG_DMA2DAD+0: dmas[2].dst.addr_b[0] = byte; break;
    case REG_DMA2DAD+1: dmas[2].dst.addr_b[1] = byte; break;
    case REG_DMA2DAD+2: dmas[2].dst.addr_b[2] = byte; break;
    case REG_DMA2DAD+3: dmas[2].dst.addr_b[3] = byte & 0x7; break;
    case REG_DMA3DAD+0: dmas[3].dst.addr_b[0] = byte; break;
    case REG_DMA3DAD+1: dmas[3].dst.addr_b[1] = byte; break;
    case REG_DMA3DAD+2: dmas[3].dst.addr_b[2] = byte; break;
    case REG_DMA3DAD+3: dmas[3].dst.addr_b[3] = byte & 0xF; break;

    case REG_DMA0CNT_L+0: dmas[0].count_b[0] = byte; break;
    case REG_DMA0CNT_L+1: dmas[0].count_b[1] = byte & 0x3F; break;
    case REG_DMA1CNT_L+0: dmas[1].count_b[0] = byte; break;
    case REG_DMA1CNT_L+1: dmas[1].count_b[1] = byte & 0x3F; break;
    case REG_DMA2CNT_L+0: dmas[2].count_b[0] = byte; break;
    case REG_DMA2CNT_L+1: dmas[2].count_b[1] = byte & 0x3F; break;
    case REG_DMA3CNT_L+0: dmas[3].count_b[0] = byte; break;
    case REG_DMA3CNT_L+1: dmas[3].count_b[1] = byte; break;

    case REG_DMA0CNT_H+0: writeDMAControlLower(dmas[0].control, byte); break;
    case REG_DMA0CNT_H+1: writeDMAControlUpper(dmas[0].control, byte); break;
    case REG_DMA1CNT_H+0: writeDMAControlLower(dmas[1].control, byte); break;
    case REG_DMA1CNT_H+1: writeDMAControlUpper(dmas[1].control, byte); break;
    case REG_DMA2CNT_H+0: writeDMAControlLower(dmas[2].control, byte); break;
    case REG_DMA2CNT_H+1: writeDMAControlUpper(dmas[2].control, byte); break;
    case REG_DMA3CNT_H+0: writeDMAControlLower(dmas[3].control, byte); break;
    case REG_DMA3CNT_H+1: writeDMAControlUpper(dmas[3].control, byte); break;

    case REG_TM0CNT_L+0: timers[0].initial_b[0] = byte; break;
    case REG_TM0CNT_L+1: timers[0].initial_b[1] = byte; break;
    case REG_TM1CNT_L+0: timers[1].initial_b[0] = byte; break;
    case REG_TM1CNT_L+1: timers[1].initial_b[1] = byte; break;
    case REG_TM2CNT_L+0: timers[2].initial_b[0] = byte; break;
    case REG_TM2CNT_L+1: timers[2].initial_b[1] = byte; break;
    case REG_TM3CNT_L+0: timers[3].initial_b[0] = byte; break;
    case REG_TM3CNT_L+1: timers[3].initial_b[1] = byte; break;

    case REG_TM0CNT_H: writeTimerControl(timers[0], byte); break;
    case REG_TM1CNT_H: writeTimerControl(timers[1], byte); break;
    case REG_TM2CNT_H: writeTimerControl(timers[2], byte); break;
    case REG_TM3CNT_H: writeTimerControl(timers[3], byte); break;

    case REG_KEYCNT:
        keycnt.keys_b[0] = byte;
        break;

    case REG_KEYCNT+1:
        keycnt.keys_b[1] = byte & 0x3;
        keycnt.irq       = bits<6, 1>(byte);
        keycnt.logic     = bits<7, 1>(byte);
        break;

    case REG_IME:
        intr_master = bits<0, 1>(byte);
        break;

    case REG_IF:
    case REG_IF+1:
        io[addr] &= ~byte;
        return;

    case REG_HALTCNT:
        halt = true;
        break;
    }
    io[addr] = byte;
}

void MMU::writeOAM(u32 addr, u16 half)
{
    addr &= 0x3FF;
    if ((addr & 0b110) != 0b110)
    {
        OAMEntry& entry = oam_entries[addr >> 3];
        switch (addr & 0x7)
        {
        case 0b000:
            entry.y           = bits< 0, 8>(half);
            entry.affine      = bits< 8, 1>(half);
            entry.double_size = bits< 9, 1>(half);
            entry.disabled    = bits< 9, 1>(half);
            entry.gfx_mode    = bits<10, 2>(half);
            entry.mosaic      = bits<12, 1>(half);
            entry.color_mode  = bits<13, 1>(half);
            entry.shape       = bits<14, 2>(half);
            break;

        case 0b010:
            entry.x        = bits< 0, 9>(half);
            entry.paramter = bits< 9, 5>(half);
            entry.flip_x   = bits<12, 1>(half);
            entry.flip_y   = bits<13, 1>(half);
            entry.size     = bits<14, 2>(half);
            break;

        case 0b100:
            entry.tile      = bits< 0, 10>(half);
            entry.priority  = bits<10,  2>(half);
            entry.palette   = bits<12,  4>(half);
            break;
        }
    }
    oam.ref<u16>(addr) = half;
}

void MMU::writeBackgroundControlLower(BackgroundControl& control, u8 byte)
{
    control.priority     = bits<0, 2>(byte);
    control.tile_block   = bits<2, 2>(byte);
    control.mosaic       = bits<6, 1>(byte);
    control.palette_type = bits<7, 1>(byte);
 } 

void MMU::writeBackgroundControlUpper(BackgroundControl& control, u8 byte)
{
    control.map_block   = bits<0, 5>(byte);
    control.wraparound  = bits<5, 1>(byte);
    control.screen_size = bits<6, 2>(byte);
}

void MMU::writeWindow(Window& window, u8 byte)
{
    window.bg0 = bits<0, 1>(byte);
    window.bg1 = bits<1, 1>(byte);
    window.bg2 = bits<2, 1>(byte);
    window.bg3 = bits<3, 1>(byte);
    window.obj = bits<4, 1>(byte);
    window.sfx = bits<5, 1>(byte);
}

void MMU::writeBlendLayer(BlendControl::Layer& layer, u8 byte)
{
    layer.bg0 = bits<0, 1>(byte);
    layer.bg1 = bits<1, 1>(byte);
    layer.bg2 = bits<2, 1>(byte);
    layer.bg3 = bits<3, 1>(byte);
    layer.obj = bits<4, 1>(byte);
    layer.bdp = bits<5, 1>(byte);
}

void MMU::writeDMAControlLower(DMA::Control& control, u8 byte)
{
    control.dst_adjust = bits<5, 2>(byte);
    control.src_adjust = (control.src_adjust & ~0x1) | bits<7, 1>(byte);
}

void MMU::writeDMAControlUpper(DMA::Control& control, u8 byte)
{
    control.src_adjust  = (control.src_adjust & ~0x2) | (bits<0, 1>(byte) << 1);
    control.repeat      = bits<1, 1>(byte);
    control.word        = bits<2, 1>(byte);
    control.gamepak_drq = bits<3, 1>(byte);
    control.timing      = bits<4, 2>(byte);
    control.irq         = bits<6, 1>(byte);
    control.enable      = bits<7, 1>(byte);

    signalDMA(DMA::Timing::IMMEDIATE);
}

void MMU::writeTimerControl(Timer& timer, u8 byte)
{
    timer.attemptInit(byte & 0x80);
    timer.control.prescaler = bits<0, 2>(byte);
    timer.control.cascade   = bits<2, 1>(byte);
    timer.control.irq       = bits<6, 1>(byte);
    timer.control.enabled   = bits<7, 1>(byte);
}
 