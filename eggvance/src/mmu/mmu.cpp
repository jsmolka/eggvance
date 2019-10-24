#include "mmu.h"

#include "arm/arm.h"
#include "ppu/ppu.h"
#include "sys/keypad.h"

MMU mmu;

void MMU::reset()
{
    bios.reset();

    ewram.fill(0);
    iwram.fill(0);
    ioram.fill(0);

    io.reset();
    palette.reset();
    vram.reset();
    oam.reset();

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
        if (addr < 0x0400'0400)
            return io.readByte(addr);
        return 0;

    case PAGE_PALETTE:
        return palette.readByte(addr);

    case PAGE_VRAM:
        return vram.readByte(addr);

    case PAGE_OAM:
        return oam.readByte(addr);

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
        if (addr < 0x0400'0400)
            return io.readHalf(addr);
        return 0;

    case PAGE_PALETTE:
        return palette.readHalf(addr);

    case PAGE_VRAM:
        return vram.readHalf(addr);

    case PAGE_OAM:
        return oam.readHalf(addr);

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
        if (addr < 0x0400'0400)
            return io.readWord(addr);
        return 0;

    case PAGE_PALETTE:
        return palette.readWord(addr);

    case PAGE_VRAM:
        return vram.readWord(addr);

    case PAGE_OAM:
        return oam.readWord(addr);

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
        if (addr < 0x0400'0400)
            io.writeByte(addr, byte);
        break;

    case PAGE_PALETTE:
        palette.writeByte(addr, byte);
        break;

    case PAGE_VRAM:
        vram.writeByte(addr, byte);
        break;

    case PAGE_OAM:
        oam.writeByte(addr, byte);
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
        if (addr < 0x0400'0400)
            io.writeHalf(addr, half);
        break;

    case PAGE_PALETTE:
        palette.writeHalf(addr, half);
        break;

    case PAGE_VRAM:
        vram.writeHalf(addr, half);
        break;

    case PAGE_OAM:
        oam.writeHalf(addr, half);
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
        if (addr < 0x0400'0400)
            io.writeWord(addr, word);
        break;

    case PAGE_PALETTE:
        palette.writeWord(addr, word);
        break;

    case PAGE_VRAM:
        vram.writeWord(addr, word);
        break;

    case PAGE_OAM:
        oam.writeWord(addr, word);
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
