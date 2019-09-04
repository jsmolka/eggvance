#include "mmu.h"

#include "common/utility.h"

MMU::MMU(std::unique_ptr<BIOS> bios)
    : bios(std::move(bios))
    , timers{ 
        Timer(0, mmio.tmcnt[0], &timers[1]), 
        Timer(1, mmio.tmcnt[1], &timers[2]), 
        Timer(2, mmio.tmcnt[2], &timers[3]), 
        Timer(3, mmio.tmcnt[3], nullptr) }
    , dmas{
        DMA(0, *this),
        DMA(1, *this),
        DMA(2, *this),
        DMA(3, *this) }
{
    dmas_active.reserve(4);

    reset();
}

void MMU::reset()
{
    bios->reset();

    ewram.fill(0);
    iwram.fill(0);
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

    mmio.reset();

    oam_entries.fill(OAMEntry());
}

void MMU::setGamePak(std::unique_ptr<GamePak> gamepak)
{
    this->gamepak = std::move(gamepak);
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

u8 MMU::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
        if (addr < 0x4000)
            return bios->readByte(addr);
        else
            return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return ewram.readByte(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return iwram.readByte(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
            return mmio.readByte(addr);
        }
        return 0;

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
            if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
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
    switch (addr >> 24)
    {
    case PAGE_BIOS:
        if (addr < 0x4000)
        {
            addr &= 0x3FFE;
            return bios->readHalf(addr);
        }
        return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFE;
        return ewram.readHalf(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFE;
        return iwram.readHalf(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            return mmio.readHalf(addr);
        }
        return 0;

    case PAGE_PALETTE:
        addr &= 0x3FE;
        return palette.readHalf(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFE;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        return vram.readHalf(addr);

    case PAGE_OAM:
        addr &= 0x3FE;
        return oam.readHalf(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFE;
        return gamepak->readHalf(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak->save->type == Save::Type::EEPROM)
        {
            if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
                return 1;
        }
        addr &= 0x1FF'FFFE;
        return gamepak->readHalf(addr);

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        return 0;
    }
    return 0;
}

u32 MMU::readWord(u32 addr)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
        if (addr < 0x4000)
        {
            addr &= 0x3FFC;
            return bios->readWord(addr);
        }
        return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFC;
        return ewram.readWord(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFC;
        return iwram.readWord(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            return mmio.readWord(addr);
        }
        return 0;

    case PAGE_PALETTE:
        addr &= 0x3FC;
        return palette.readWord(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFC;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        return vram.readWord(addr);

    case PAGE_OAM:
        addr &= 0x3FC;
        return oam.readWord(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFC;
        return gamepak->readWord(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak->save->type == Save::Type::EEPROM)
        {
            if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
                return 1;
        }
        addr &= 0x1FF'FFFC;
        return gamepak->readWord(addr);

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        return 0;
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
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
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
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFE;
        ewram.writeHalf(addr, half);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFE;
        iwram.writeHalf(addr, half);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            mmio.writeHalf(addr, half);

            switch (addr)
            {
            case REG_DMA0CNT_H:
            case REG_DMA1CNT_H:
            case REG_DMA2CNT_H:
            case REG_DMA3CNT_H:
                signalDMA(DMA::Timing::IMMEDIATE);
                break;
            }
        }
        break;

    case PAGE_PALETTE:
        addr &= 0x3FE;
        palette.writeHalf(addr, half);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFE;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        vram.writeHalf(addr, half);
        break;

    case PAGE_OAM:
        addr &= 0x3FE;
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
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFC;
        ewram.writeWord(addr, word);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFC;
        iwram.writeWord(addr, word);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            mmio.writeWord(addr, word);

            switch (addr)
            {
            case REG_DMA0CNT_L:
            case REG_DMA1CNT_L:
            case REG_DMA2CNT_L:
            case REG_DMA3CNT_L:
                signalDMA(DMA::Timing::IMMEDIATE);
                break;
            }
        }
        break;

    case PAGE_PALETTE:
        addr &= 0x3FC;
        palette.writeWord(addr, word);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFC;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        vram.writeWord(addr, word);
        break;

    case PAGE_OAM:
        addr &= 0x3FC;
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

void MMU::writeOAM(u32 addr, u16 half)
{
    if ((addr & 0b110) != 0b110)
    {
        auto& entry = oam_entries[addr >> 3];
        switch (addr & 0x7)
        {
        case 0: entry.write<0>(half); break;
        case 2: entry.write<2>(half); break;
        case 4: entry.write<4>(half); break;
        }
    }
    oam.writeHalf(addr, half);
}
