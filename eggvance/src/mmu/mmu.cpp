#include "mmu.h"

#include "common/format.h"
#include "common/utility.h"

MMU::MMU(std::unique_ptr<BIOS> bios)
    : bios(std::move(bios))
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
    mmio.dispstat.hblank = false;
    mmio.dispstat.vblank = false;

    mmio.keyinput = 0x3FF;
    mmio.halt = false;

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
        return bios->readByte(addr);

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.readByte(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.readByte(addr);

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
            if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xDFF'FFFF))
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
        return bios->readHalf(addr);

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.readHalf(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.readHalf(addr);

    case PAGE_IO:
        return readIO(addr) 
             | readIO(addr + 1) << 8;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        return palette.readHalf(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        return vram.readHalf(addr);

    case PAGE_OAM:
        addr &= 0x3FF;
        return oam.readHalf(addr);

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
            if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xDFF'FFFF))
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
        return bios->readWord(addr);

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.readWord(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.readWord(addr);

    case PAGE_IO:
        return readIO(addr) 
             | readIO(addr + 1) << 8
             | readIO(addr + 2) << 16
             | readIO(addr + 3) << 24;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        return palette.readWord(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        return vram.readWord(addr);

    case PAGE_OAM:
        addr &= 0x3FF;
        return oam.readWord(addr);

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
        ewram.writeByte(addr, byte);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram.writeByte(addr, byte);
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
        ewram.writeHalf(addr, half);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram.writeHalf(addr, half);
        break;

    case PAGE_IO:
        writeIO(addr + 0, (half >> 0) & 0xFF);
        writeIO(addr + 1, (half >> 8) & 0xFF);
        break;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        palette.writeHalf(addr, half);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        vram.writeHalf(addr, half);
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
        ewram.writeWord(addr, word);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        iwram.writeWord(addr, word);
        break;

    case PAGE_IO:
        writeIO(addr + 0, (word >>  0) & 0xFF);
        writeIO(addr + 1, (word >>  8) & 0xFF);
        writeIO(addr + 2, (word >> 16) & 0xFF);
        writeIO(addr + 3, (word >> 24) & 0xFF);
        break;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        palette.writeWord(addr, word);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr > 0x1'7FFF)
            addr -= 0x8000;
        vram.writeWord(addr, word);
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

void MMU::signalDMA(DMA::Timing timing)
{
    bool pushed = false;
    for (DMA& dma : dmas)
    {
        if (!dma.active && dma.control.enable && dma.control.timing == timing)
        {
            dma.activate();
            if (dma.active)
            {
                dmas_active.push_back(&dma);
                pushed = true;
            }
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
    case REG_TM0CNT_L+0: return timers[0].data_b[0];
    case REG_TM0CNT_L+1: return timers[0].data_b[1];
    case REG_TM1CNT_L+0: return timers[1].data_b[0];
    case REG_TM1CNT_L+1: return timers[1].data_b[1];
    case REG_TM2CNT_L+0: return timers[2].data_b[0];
    case REG_TM2CNT_L+1: return timers[2].data_b[1];
    case REG_TM3CNT_L+0: return timers[3].data_b[0];
    case REG_TM3CNT_L+1: return timers[3].data_b[1];
    }
    return mmio.readByte(addr);
}

void MMU::writeIO(u32 addr, u8 byte)
{
    if (addr >= (MAP_IO + 0x400))
        return;

    addr &= 0x3FF;
    switch (addr)
    {
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
    }
    mmio.writeByte(addr, byte);

    switch (addr)
    {
    case REG_DMA0CNT_H+1:
    case REG_DMA1CNT_H+1:
    case REG_DMA2CNT_H+1:
    case REG_DMA3CNT_H+1:
        signalDMA(DMA::Timing::IMMEDIATE);
        break;
    }
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
    oam.writeHalf(addr, half);
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

void MMU::writeTimerControl(Timer& timer, u8 byte)
{
    timer.attemptInit(byte & 0x80);
    timer.control.prescaler = bits<0, 2>(byte);
    timer.control.cascade   = bits<2, 1>(byte);
    timer.control.irq       = bits<6, 1>(byte);
    timer.control.enabled   = bits<7, 1>(byte);
}
 