#include "mmu.h"

#include "arm/arm.h"
#include "common/utility.h"
#include "memmap.h"

MMU mmu;

void MMU::reset()
{
    bios.reset();
    palette.reset();
    vram.reset();
    oam.reset();
    io.reset();

    ewram.fill(0);
    iwram.fill(0);
}

u8 MMU::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case REGION_BIOS:
        if (addr < 0x4000)
            return bios.readByte(addr);
        else
            return readUnused(addr);

    case REGION_EWRAM:
        return ewram.readByte(addr);

    case REGION_IWRAM:
        return iwram.readByte(addr);

    case REGION_IO:
        return io.readByte(addr);

    case REGION_PALETTE:
        return palette.readByte(addr);

    case REGION_VRAM:
        return vram.readByte(addr);

    case REGION_OAM:
        return oam.readByte(addr);

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
        addr &= 0x1FF'FFFF;
        return gamepak.readByte(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            if (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00)
                return 1;
        }
        addr &= 0x1FF'FFFF;
        return gamepak.readByte(addr);
    
    case REGION_SRAM:
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

    case REGION_SRAM_MIRROR:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            return gamepak.backup->readByte(addr);
        }
        return 0;
    }
    return readUnused(addr);
}

u16 MMU::readHalf(u32 addr)
{
    switch (addr >> 24)
    {
    case REGION_BIOS:
        if (addr < 0x4000)
            return bios.readHalf(addr);
        else
            return readUnused(addr);

    case REGION_EWRAM:
        return ewram.readHalf(addr);

    case REGION_IWRAM:
        return iwram.readHalf(addr);

    case REGION_IO:
        return io.readHalf(addr);

    case REGION_PALETTE:
        return palette.readHalf(addr);

    case REGION_VRAM:
        return vram.readHalf(addr);

    case REGION_OAM:
        return oam.readHalf(addr);

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
        addr &= 0x1FF'FFFE;
        return gamepak.readHalf(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            if (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00)
                return 1;
        }
        addr &= 0x1FF'FFFE;
        return gamepak.readHalf(addr);

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            return gamepak.backup->readByte(addr) * 0x0101;
        }
        return 0;
    }
    return readUnused(addr);
}

u32 MMU::readWord(u32 addr)
{
    switch (addr >> 24)
    {
    case REGION_BIOS:
        if (addr < 0x4000)
            return bios.readWord(addr);
        else
            return readUnused(addr);

    case REGION_EWRAM:
        return ewram.readWord(addr);

    case REGION_IWRAM:
        return iwram.readWord(addr);

    case REGION_IO:
        return io.readWord(addr);

    case REGION_PALETTE:
        return palette.readWord(addr);

    case REGION_VRAM:
        return vram.readWord(addr);

    case REGION_OAM:
        return oam.readWord(addr);

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
        addr &= 0x1FF'FFFC;
        return gamepak.readWord(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.backup->type == Backup::Type::EEPROM)
        {
            if (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00)
                return 1;
        }
        addr &= 0x1FF'FFFC;
        return gamepak.readWord(addr);

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            return gamepak.backup->readByte(addr) * 0x01010101;
        }
        return 0;
    }
    return readUnused(addr);
}

void MMU::writeByte(u32 addr, u8 byte)
{
    switch (addr >> 24)
    {
    case REGION_BIOS:
        break;

    case REGION_EWRAM:
        ewram.writeByte(addr, byte);
        break;

    case REGION_IWRAM:
        iwram.writeByte(addr, byte);
        break;

    case REGION_IO:
        io.writeByte(addr, byte);
        break;

    case REGION_PALETTE:
        palette.writeByte(addr, byte);
        break;

    case REGION_VRAM:
        vram.writeByte(addr, byte);
        break;

    case REGION_OAM:
        break;

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
    case REGION_GAMEPAK2_EX:
        break;

    case REGION_SRAM:
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

    case REGION_SRAM_MIRROR:
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
    case REGION_BIOS:
        break;

    case REGION_EWRAM:
        ewram.writeHalf(addr, half);
        break;

    case REGION_IWRAM:
        iwram.writeHalf(addr, half);
        break;

    case REGION_IO:
        io.writeHalf(addr, half);
        break;

    case REGION_PALETTE:
        palette.writeHalf(addr, half);
        break;

    case REGION_VRAM:
        vram.writeHalf(addr, half);
        break;

    case REGION_OAM:
        oam.writeHalf(addr, half);
        break;

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
    case REGION_GAMEPAK2_EX:
        break;

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            half = rotateRight(half, (addr & 0x3) << 3);
            gamepak.backup->writeByte(addr, half & 0xFF);
        }
        break;
    }
}

void MMU::writeWord(u32 addr, u32 word)
{
    switch (addr >> 24)
    {
    case REGION_BIOS:
        break;

    case REGION_EWRAM:
        ewram.writeWord(addr, word);
        break;

    case REGION_IWRAM:
        iwram.writeWord(addr, word);
        break;

    case REGION_IO:
        io.writeWord(addr, word);
        break;

    case REGION_PALETTE:
        palette.writeWord(addr, word);
        break;

    case REGION_VRAM:
        vram.writeWord(addr, word);
        break;

    case REGION_OAM:
        oam.writeWord(addr, word);
        break;

    case REGION_GAMEPAK0:
    case REGION_GAMEPAK0_EX:
    case REGION_GAMEPAK1:
    case REGION_GAMEPAK1_EX:
    case REGION_GAMEPAK2:
    case REGION_GAMEPAK2_EX:
        break;

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        if (gamepak.backup->type == Backup::Type::SRAM)
        {
            addr &= 0x7FFF;
            word = rotateRight(word, (addr & 0x3) << 3);
            gamepak.backup->writeByte(addr, word & 0xFF);
        }
        break;
    }
}

u32 MMU::readUnused(u32 addr)
{
    u32 value = 0;
    if (arm.cpsr.thumb)
    {
        u32 lsw = arm.pipe[1];
        u32 msw = arm.pipe[1];

        switch (addr >> 24)
        {
        case REGION_BIOS:
        case REGION_OAM:
            lsw = arm.pipe[0];
            break;

        case REGION_IWRAM:
            if (misalignedWord(addr))
                lsw = arm.pipe[0];
            else
                msw = arm.pipe[0];
            break;
        }
        value = (msw << 16) | lsw;
    }
    else
    {
        value = arm.pipe[1];
    }
    return value >> ((addr & 0x3) << 3);
}
