#include "mmu.h"

#include "arm/arm.h"
#include "base/log.h"
#include "gamepak/gamepak.h"

enum Region
{
    kRegionBios,
    kRegionUnused,
    kRegionEwram,
    kRegionIwram,
    kRegionIo,
    kRegionPram,
    kRegionVram,
    kRegionOam,
    kRegionGamePak0L,
    kRegionGamePak0H,
    kRegionGamePak1L,
    kRegionGamePak1H,
    kRegionGamePak2L,
    kRegionGamePak2H,
    kRegionSaveL,
    kRegionSaveH
};

u8 Mmu::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        return ewram.readByte(addr);

    case kRegionIwram:
        return iwram.readByte(addr);

    case kRegionIo:
        return mmio.readByte(addr);

    case kRegionPram:
        return pram.readByte(addr);

    case kRegionVram:
        return vram.readByte(addr);

    case kRegionOam:
        return oam.readByte(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        return gamepak.readByte(addr);
    
    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr);

    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readByte(addr);
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad read {:08X}", addr);
        return readUnused(addr);
    }
}

u16 Mmu::readHalf(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        return ewram.readHalf(addr);

    case kRegionIwram:
        return iwram.readHalf(addr);

    case kRegionIo:
        return mmio.readHalf(addr);

    case kRegionPram:
        return pram.readHalf(addr);

    case kRegionVram:
        return vram.readHalf(addr);

    case kRegionOam:
        return oam.readHalf(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
            return 1;
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        return gamepak.readHalf(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr) * 0x0101;

    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readHalf(addr);
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad read {:08X}", addr);
        return readUnused(addr);
    }
}

u32 Mmu::readWord(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        return ewram.readWord(addr);

    case kRegionIwram:
        return iwram.readWord(addr);

    case kRegionIo:
        return mmio.readWord(addr);

    case kRegionPram:
        return pram.readWord(addr);

    case kRegionVram:
        return vram.readWord(addr);

    case kRegionOam:
        return oam.readWord(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
            return 1;
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        return gamepak.readWord(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr) * 0x0101'0101;

    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readWord(addr);
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad read {:08X}", addr);
        return readUnused(addr);
    }
}

void Mmu::writeByte(u32 addr, u8 byte)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        ewram.writeByte(addr, byte);
        break;

    case kRegionIwram:
        iwram.writeByte(addr, byte);
        break;

    case kRegionIo:
        mmio.writeByte(addr, byte);
        break;

    case kRegionPram:
        pram.writeByte(addr, byte);
        break;

    case kRegionVram:
        vram.writeByte(addr, byte);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.writeByte(addr, byte);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, byte);
        break;

    case kRegionBios:
    case kRegionOam:
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad write {:08X} -> {:02X}", addr, byte);
        break;
    }
}

void Mmu::writeHalf(u32 addr, u16 half)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        ewram.writeHalf(addr, half);
        break;

    case kRegionIwram:
        iwram.writeHalf(addr, half);
        break;

    case kRegionIo:
        mmio.writeHalf(addr, half);
        break;

    case kRegionPram:
        pram.writeHalf(addr, half);
        break;

    case kRegionVram:
        vram.writeHalf(addr, half);
        break;

    case kRegionOam:
        oam.writeHalf(addr, half);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.writeHalf(addr, half);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, half >> (8 * (addr & 0x1)));
        break;

    case kRegionBios:
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad write {:08X} -> {:04X}", addr, half);
        break;
    }
}

void Mmu::writeWord(u32 addr, u32 word)
{
    switch (addr >> 24)
    {
    case kRegionEwram:
        ewram.writeWord(addr, word);
        break;

    case kRegionIwram:
        iwram.writeWord(addr, word);
        break;

    case kRegionIo:
        mmio.writeWord(addr, word);
        break;

    case kRegionPram:
        pram.writeWord(addr, word);
        break;

    case kRegionVram:
        vram.writeWord(addr, word);
        break;

    case kRegionOam:
        oam.writeWord(addr, word);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.writeWord(addr, word);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, word >> (8 * (addr & 0x3)));
        break;

    case kRegionBios:
        [[fallthrough]];

    default:
        SHELL_LOG_WARN("Bad write {:08X} -> {:08X}", addr, word);
        break;
    }
}

u32 Mmu::readUnused(u32 addr) const
{
    u32 value = 0;
    if (arm.cpsr.t)
    {
        u32 lsw = arm.pipe[1];
        u32 msw = arm.pipe[1];

        switch (addr >> 24)
        {
        case kRegionBios:
        case kRegionOam:
            lsw = arm.pipe[0];
            break;

        case kRegionIwram:
            if (addr & 0x3)
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
    return bit::ror(value, 8 * (addr & 0x3));
}

u8 Mmu::readSave(u32 addr)
{
    switch (gamepak.save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        return gamepak.save->read(addr);

    case Save::Type::Flash512:
    case Save::Type::Flash1024:
        addr &= 0xFFFF;
        return gamepak.save->read(addr);

    default:
        SHELL_LOG_WARN("Bad read {:08X}", addr);
        return 0xFF;
    }
}

void Mmu::writeSave(u32 addr, u8 byte)
{
    switch (gamepak.save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        gamepak.save->write(addr, byte);
        break;

    case Save::Type::Flash512:
    case Save::Type::Flash1024:
        addr &= 0xFFFF;
        gamepak.save->write(addr, byte);
        break;

    default:
        SHELL_LOG_WARN("Bad write {:08X} -> {:02X}", addr, byte);
        break;
    }
}
