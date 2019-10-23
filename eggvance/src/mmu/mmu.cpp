#include "mmu.h"

#include "arm/arm.h"
#include "ppu/ppu.h"
#include "keypad.h"

MMU mmu;

void MMU::reset()
{
    bios.reset();

    ewram.fill(0);
    iwram.fill(0);
    ioram.fill(0);

    ioram.writeHalf(REG_KEYINPUT, 0xFFFF);
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
            return readByteIO(addr);
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
            return readByteIO(addr + 0) << 0
                 | readByteIO(addr + 1) << 8;
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
        return ewram.readWord(addr);

    case PAGE_IWRAM:
        return iwram.readWord(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            return readByteIO(addr + 0) <<  0
                 | readByteIO(addr + 1) <<  8
                 | readByteIO(addr + 2) << 16
                 | readByteIO(addr + 3) << 24;
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
            writeByteIO(addr, byte);
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
            writeByteIO(addr + 0, (half >> 0) & 0xFF);
            writeByteIO(addr + 1, (half >> 8) & 0xFF);
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
            writeByteIO(addr + 0, (word >>  0) & 0xFF);
            writeByteIO(addr + 1, (word >>  8) & 0xFF);
            writeByteIO(addr + 2, (word >> 16) & 0xFF);
            writeByteIO(addr + 3, (word >> 24) & 0xFF);
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

#define READ2(label, reg)                      \
    case label + 0: return reg.readByte<0>();  \
    case label + 1: return reg.readByte<1>()

#define READ4(label, reg)                      \
    case label + 0: return reg.readByte<0>();  \
    case label + 1: return reg.readByte<1>();  \
    case label + 2: return reg.readByte<2>();  \
    case label + 3: return reg.readByte<3>()

u8 MMU::readByteIO(u32 addr)
{
    if (addr >= REG_DMA0SAD && addr < REG_TM0CNT)
    {
        return arm.dma.readByte(addr);
    }

    switch (addr)
    {
    READ2(REG_DISPCNT,  ppu.io.dispcnt);
    READ2(REG_DISPSTAT, ppu.io.dispstat);
    READ2(REG_VCOUNT,   ppu.io.vcount);
    READ2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    READ2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    READ2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    READ2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    READ2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    READ2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    READ2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    READ2(REG_BG3HOFS,  ppu.io.bghofs[3]);
    READ2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    READ2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    READ2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    READ2(REG_BG3VOFS,  ppu.io.bgvofs[3]);
    READ2(REG_BG2PA,    ppu.io.bgpa[0]);
    READ2(REG_BG2PB,    ppu.io.bgpb[0]);
    READ2(REG_BG2PC,    ppu.io.bgpc[0]);
    READ2(REG_BG2PD,    ppu.io.bgpd[0]);
    READ2(REG_BG3PA,    ppu.io.bgpa[1]);
    READ2(REG_BG3PB,    ppu.io.bgpb[1]);
    READ2(REG_BG3PC,    ppu.io.bgpc[1]);
    READ2(REG_BG3PD,    ppu.io.bgpd[1]);
    READ4(REG_BG2X,     ppu.io.bgx[0]);
    READ4(REG_BG2Y,     ppu.io.bgy[0]);
    READ4(REG_BG3X,     ppu.io.bgx[1]);
    READ4(REG_BG3Y,     ppu.io.bgy[1]);
    READ2(REG_WIN0H,    ppu.io.winh[0]);
    READ2(REG_WIN0V,    ppu.io.winv[0]);
    READ2(REG_WIN1H,    ppu.io.winh[1]);
    READ2(REG_WIN1V,    ppu.io.winv[1]);
    READ2(REG_WININ,    ppu.io.winin);
    READ2(REG_WINOUT,   ppu.io.winout);
    READ2(REG_MOSAIC,   ppu.io.mosaic);
    READ2(REG_BLDCNT,   ppu.io.bldcnt);
    READ2(REG_BLDALPHA, ppu.io.bldalpha);
    READ2(REG_BLDY,     ppu.io.bldy);
    READ2(REG_KEYINPUT, keypad.io.keyinput);
    READ2(REG_KEYCNT,   keypad.io.keycnt);
    READ2(REG_IME,      arm.io.int_master);
    READ2(REG_IE,       arm.io.int_enabled);
    READ2(REG_IF,       arm.io.int_request);
    READ4(REG_DMA0SAD,  arm.dma.dmas[0].sad);
    READ4(REG_DMA1SAD,  arm.dma.dmas[1].sad);
    READ4(REG_TM0CNT,   arm.timers[0]);
    READ4(REG_TM1CNT,   arm.timers[1]);
    READ4(REG_TM2CNT,   arm.timers[2]);
    READ4(REG_TM3CNT,   arm.timers[3]);
    READ2(REG_WAITCNT,  arm.io.waitcnt);
    }
    return ioram.readByte(addr);
}

#undef READ4
#undef READ2

#define WRITE2(label, reg)                           \
    case label + 0: reg.writeByte<0>(byte); return;  \
    case label + 1: reg.writeByte<1>(byte); return

#define WRITE4(label, reg)                           \
    case label + 0: reg.writeByte<0>(byte); return;  \
    case label + 1: reg.writeByte<1>(byte); return;  \
    case label + 2: reg.writeByte<2>(byte); return;  \
    case label + 3: reg.writeByte<3>(byte); return

void MMU::writeByteIO(u32 addr, u8 byte)
{
    if (addr >= REG_DMA0SAD && addr < REG_TM0CNT)
    {
        arm.dma.writeByte(addr, byte);
        return;
    }

    switch (addr)
    {
    WRITE2(REG_DISPCNT,  ppu.io.dispcnt);
    WRITE2(REG_DISPSTAT, ppu.io.dispstat);
    WRITE2(REG_VCOUNT,   ppu.io.vcount);
    WRITE2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    WRITE2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    WRITE2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    WRITE2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    WRITE2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    WRITE2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    WRITE2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    WRITE2(REG_BG3HOFS,  ppu.io.bghofs[3]);
    WRITE2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    WRITE2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    WRITE2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    WRITE2(REG_BG3VOFS,  ppu.io.bgvofs[3]);
    WRITE2(REG_BG2PA,    ppu.io.bgpa[0]);
    WRITE2(REG_BG2PB,    ppu.io.bgpb[0]);
    WRITE2(REG_BG2PC,    ppu.io.bgpc[0]);
    WRITE2(REG_BG2PD,    ppu.io.bgpd[0]);
    WRITE2(REG_BG3PA,    ppu.io.bgpa[1]);
    WRITE2(REG_BG3PB,    ppu.io.bgpb[1]);
    WRITE2(REG_BG3PC,    ppu.io.bgpc[1]);
    WRITE2(REG_BG3PD,    ppu.io.bgpd[1]);
    WRITE4(REG_BG2X,     ppu.io.bgx[0]);
    WRITE4(REG_BG2Y,     ppu.io.bgy[0]);
    WRITE4(REG_BG3X,     ppu.io.bgx[1]);
    WRITE4(REG_BG3Y,     ppu.io.bgy[1]);
    WRITE2(REG_WIN0H,    ppu.io.winh[0]);
    WRITE2(REG_WIN0V,    ppu.io.winv[0]);
    WRITE2(REG_WIN1H,    ppu.io.winh[1]);
    WRITE2(REG_WIN1V,    ppu.io.winv[1]);
    WRITE2(REG_WININ,    ppu.io.winin);
    WRITE2(REG_WINOUT,   ppu.io.winout);
    WRITE2(REG_MOSAIC,   ppu.io.mosaic);
    WRITE2(REG_BLDCNT,   ppu.io.bldcnt);
    WRITE2(REG_BLDALPHA, ppu.io.bldalpha);
    WRITE2(REG_BLDY,     ppu.io.bldy);
    WRITE2(REG_KEYINPUT, keypad.io.keyinput);
    WRITE2(REG_KEYCNT,   keypad.io.keycnt);
    WRITE2(REG_IME,      arm.io.int_master);
    WRITE2(REG_IE,       arm.io.int_enabled);
    WRITE2(REG_IF,       arm.io.int_request);
    WRITE4(REG_TM0CNT,   arm.timers[0]);
    WRITE4(REG_TM1CNT,   arm.timers[1]);
    WRITE4(REG_TM2CNT,   arm.timers[2]);
    WRITE4(REG_TM3CNT,   arm.timers[3]);
    WRITE2(REG_WAITCNT,  arm.io.waitcnt);

    case REG_HALTCNT:
        arm.io.halt = true;
        break;
    }
    ioram.writeByte(addr, byte);
}

#undef WRITE4
#undef WRITE2
