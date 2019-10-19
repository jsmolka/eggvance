#include "mmu.h"

#include "common/utility.h"
#include "ppu/ppu.h"

MMU mmu;

void MMU::reset()
{
    bios.reset();

    ewram.fill(0);
    iwram.fill(0);
}

u8 MMU::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
        if (addr < 0x4000)
            return bios.readByte(addr);
        else
            return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        return ewram.readByte(addr);

    case PAGE_IWRAM:
        return iwram.readByte(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
            //return mmio.readByte(addr);
        }
        return 0;

    case PAGE_PALETTE:
        return ppu.palette.readByte(addr);

    case PAGE_VRAM:
        return ppu.vram.readByte(addr);

    case PAGE_OAM:
        return ppu.oam.readByte(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFF;
        return gamepak.readByte(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            // Todo: DMA running?
            if (gamepak.size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
                return 1;
        }
        addr &= 0x1FF'FFFF;
        return gamepak.readByte(addr);
    
    case PAGE_GAMEPAK_SRAM:
        switch (gamepak.backup->type)
        {
        case Backup::Type::SRAM:
            addr &= 0x7FFF;
            return gamepak.backup->readByte(addr);

        case Backup::Type::FLASH64:
        case Backup::Type::FLASH128:
            addr &= 0xFFFF;
            return gamepak.backup->readByte(addr);
        }
        return 0;

    default:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            return gamepak.backup->readByte(addr);
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
            return bios.readHalf(addr);
        else
            return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        return ewram.readHalf(addr);

    case PAGE_IWRAM:
        return iwram.readHalf(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            //return mmio.readHalf(addr);
        }
        return 0;

    case PAGE_PALETTE:
        return ppu.palette.readHalf(addr);

    case PAGE_VRAM:
        return ppu.vram.readHalf(addr);

    case PAGE_OAM:
        return ppu.oam.readHalf(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFE;
        return gamepak.readHalf(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            if (gamepak.size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
                return 1;
        }
        addr &= 0x1FF'FFFE;
        return gamepak.readHalf(addr);

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
            return bios.readWord(addr);
        else
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
            //return mmio.readWord(addr);
        }
        return 0;

    case PAGE_PALETTE:
        return ppu.palette.readWord(addr);

    case PAGE_VRAM:
        return ppu.vram.readWord(addr);

    case PAGE_OAM:
        return ppu.oam.readWord(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFC;
        return gamepak.readWord(addr);

    case PAGE_GAMEPAK_2+1:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            if (gamepak.size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
                return 1;
        }
        addr &= 0x1FF'FFFC;
        return gamepak.readWord(addr);

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
        ewram.writeByte(addr, byte);
        break;

    case PAGE_IWRAM:
        iwram.writeByte(addr, byte);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
            //mmio.writeByte(addr, byte);

            //switch (addr)
            //{
            //case REG_DMA0CNT_H+1:
            //case REG_DMA1CNT_H+1:
            //case REG_DMA2CNT_H+1:
            //case REG_DMA3CNT_H+1:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        ppu.palette.writeByte(addr, byte);
        break;

    case PAGE_VRAM:
        ppu.vram.writeByte(addr, byte);
        break;

    case PAGE_OAM:
        ppu.oam.writeByte(addr, byte);
        break;

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
    case PAGE_GAMEPAK_2+1:
        break;

    case PAGE_GAMEPAK_SRAM:
        switch (gamepak.backup->type)
        {
        case Backup::Type::SRAM:
            addr &= 0x7FFF;
            gamepak.backup->writeByte(addr, byte);
            break;

        case Backup::Type::FLASH64:
        case Backup::Type::FLASH128:
            addr &= 0xFFFF;
            gamepak.backup->writeByte(addr, byte);
            break;
        }
        break;

    default:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            gamepak.backup->writeByte(addr, byte);
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
        ewram.writeHalf(addr, half);
        break;

    case PAGE_IWRAM:
        iwram.writeHalf(addr, half);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            //mmio.writeHalf(addr, half);

            //switch (addr)
            //{
            //case REG_DMA0CNT_H:
            //case REG_DMA1CNT_H:
            //case REG_DMA2CNT_H:
            //case REG_DMA3CNT_H:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        ppu.palette.writeHalf(addr, half);
        break;

    case PAGE_VRAM:
        ppu.vram.writeHalf(addr, half);
        break;

    case PAGE_OAM:
        ppu.oam.writeHalf(addr, half);
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
        ewram.writeWord(addr, word);
        break;

    case PAGE_IWRAM:
        iwram.writeWord(addr, word);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            //mmio.writeWord(addr, word);

            //switch (addr)
            //{
            //case REG_DMA0CNT_L:
            //case REG_DMA1CNT_L:
            //case REG_DMA2CNT_L:
            //case REG_DMA3CNT_L:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        ppu.palette.writeWord(addr, word);
        break;

    case PAGE_VRAM:
        ppu.vram.writeWord(addr, word);
        break;

    case PAGE_OAM:
        ppu.oam.writeWord(addr, word);
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

#define READ_REG(label, reg, index)  \
    case label + index:              \
        return reg.readByte(index)

#define READ_REG2(label, reg)  \
    READ_REG(label, reg, 0);   \
    READ_REG(label, reg, 1)

#define READ_REG4(label, reg)  \
    READ_REG(label, reg, 0);   \
    READ_REG(label, reg, 1);   \
    READ_REG(label, reg, 2);   \
    READ_REG(label, reg, 3);

u8 MMU::readByteIO(u32 addr)
{
    switch (addr)
    {
    case REG_VCOUNT:
        return ppu.io.vcount;

    READ_REG2(REG_DISPCNT,  ppu.io.dispcnt);
    READ_REG2(REG_DISPSTAT, ppu.io.dispstat);
    READ_REG2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    READ_REG2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    READ_REG2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    READ_REG2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    READ_REG2(REG_WININ,    ppu.io.winin);
    READ_REG2(REG_WINOUT,   ppu.io.winout);
    READ_REG2(REG_BLDCNT,   ppu.io.bldcnt);
    READ_REG2(REG_BLDALPHA, ppu.io.bldalpha);
    }
    return ioram.readByte(addr);
}

#undef READ_REG
#undef READ_REG2
#undef READ_REG4

#define WRITE_REG(label, reg, index)  \
    case label + index:               \
        reg.writeByte(index, byte);   \
        break

#define WRITE_REG2(label, reg)  \
    WRITE_REG(label, reg, 0);   \
    WRITE_REG(label, reg, 1)

#define WRITE_REG4(label, reg)  \
    WRITE_REG(label, reg, 0);   \
    WRITE_REG(label, reg, 1);   \
    WRITE_REG(label, reg, 2);   \
    WRITE_REG(label, reg, 3);

void MMU::writeByteIO(u32 addr, u8 byte)
{
    switch (addr)
    {
    case REG_BLDY:
        ppu.io.bldy.write(byte);
        break;

    WRITE_REG2(REG_DISPCNT,  ppu.io.dispcnt);
    WRITE_REG2(REG_DISPSTAT, ppu.io.dispstat);
    WRITE_REG2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    WRITE_REG2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    WRITE_REG2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    WRITE_REG2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    WRITE_REG2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    WRITE_REG2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    WRITE_REG2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    WRITE_REG2(REG_BG3HOFS,  ppu.io.bghofs[3]);
    WRITE_REG2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    WRITE_REG2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    WRITE_REG2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    WRITE_REG2(REG_BG3VOFS,  ppu.io.bgvofs[3]);
    WRITE_REG2(REG_BG2PA,    ppu.io.bgpa[0]);
    WRITE_REG2(REG_BG2PB,    ppu.io.bgpb[0]);
    WRITE_REG2(REG_BG2PC,    ppu.io.bgpc[0]);
    WRITE_REG2(REG_BG2PD,    ppu.io.bgpd[0]);
    WRITE_REG2(REG_BG3PA,    ppu.io.bgpa[1]);
    WRITE_REG2(REG_BG3PB,    ppu.io.bgpb[1]);
    WRITE_REG2(REG_BG3PC,    ppu.io.bgpc[1]);
    WRITE_REG2(REG_BG3PD,    ppu.io.bgpd[1]);
    WRITE_REG4(REG_BG2X,     ppu.io.bgx[0]);
    WRITE_REG4(REG_BG2Y,     ppu.io.bgy[0]);
    WRITE_REG4(REG_BG3X,     ppu.io.bgx[1]);
    WRITE_REG4(REG_BG3Y,     ppu.io.bgy[1]);
    WRITE_REG2(REG_WIN0H,    ppu.io.winh[0]);
    WRITE_REG2(REG_WIN0V,    ppu.io.winv[0]);
    WRITE_REG2(REG_WIN1H,    ppu.io.winh[1]);
    WRITE_REG2(REG_WIN1V,    ppu.io.winv[1]);
    WRITE_REG2(REG_WININ,    ppu.io.winin);
    WRITE_REG2(REG_WINOUT,   ppu.io.winout);
    WRITE_REG2(REG_MOSAIC,   ppu.io.mosaic);
    WRITE_REG2(REG_BLDCNT,   ppu.io.bldcnt);
    WRITE_REG2(REG_BLDALPHA, ppu.io.bldalpha);
    }

    ioram.writeByte(addr, byte);
}

#undef WRITE_REG
#undef WRITE_REG2
#undef WRITE_REG4
