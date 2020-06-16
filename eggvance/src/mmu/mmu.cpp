#include "mmu.h"

#include "memmap.h"
#include "arm/arm.h"
#include "base/bits.h"

MMU mmu;

void MMU::reset()
{
    bios.reset();
    palette.reset();
    vram.reset();
    oam.reset();
    io = IO();

    ewram = decltype(ewram)();
    iwram = decltype(iwram)();
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
        return gamepak.readByte(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readByte(addr);
    
    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        return readSave(addr);

    default:
        return readUnused(addr);
    }
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
        return gamepak.readHalf(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readHalf(addr);

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        return readSave(addr) * 0x0101;

    default:
        return readUnused(addr);
    }
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
        return gamepak.readWord(addr);

    case REGION_GAMEPAK2_EX:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readWord(addr);

    case REGION_SRAM:
    case REGION_SRAM_MIRROR:
        return readSave(addr) * 0x01010101;

    default:
        return readUnused(addr);
    }
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
    case REGION_SRAM_MIRROR:
        writeSave(addr, byte);
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
        writeSave(addr, half >> ((addr & 0x1) << 3));
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
        writeSave(addr, word >> ((addr & 0x3) << 3));
        break;
    }
}

u32 MMU::readUnused(u32 addr)
{
    u32 value = 0;
    if (arm.cpsr.t)
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
    return value >> ((addr & 0x3) << 3);
}

u8 MMU::readSave(u32 addr)
{
    switch (gamepak.save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        return gamepak.save->read(addr);

    case Save::Type::Flash64:
    case Save::Type::Flash128:
        addr &= 0xFFFF;
        return gamepak.save->read(addr);

    default:
        return 0xFF;
    }
}

void MMU::writeSave(u32 addr, u8 byte)
{
    switch (gamepak.save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        gamepak.save->write(addr, byte);
        break;

    case Save::Type::Flash64:
    case Save::Type::Flash128:
        addr &= 0xFFFF;
        gamepak.save->write(addr, byte);
        break;
    }
}
