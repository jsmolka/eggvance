#include "mmu.h"

#include "base/bits.h"
#include "base/constants.h"
#include "base/util.h"
#include "core/core.h"

MMU::MMU(Core& core)
    : core(core)
    , vram(core)
    , bios(core)
    , io(core)
{

}

void MMU::reset()
{
    bios.reset();
    palette.reset();
    vram.reset();
    oam.reset();

    util::reconstruct(&io, core);

    ewram = decltype(ewram)();
    iwram = decltype(iwram)();
}

u8 MMU::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < 0x4000)
            return bios.readByte(addr);
        else
            return readUnused(addr);

    case kRegionEwram:
        return ewram.readByte(addr);

    case kRegionIwram:
        return iwram.readByte(addr);

    case kRegionIo:
        return io.readByte(addr);

    case kRegionPalette:
        return palette.readByte(addr);

    case kRegionVram:
        return vram.readByte(addr);

    case kRegionOam:
        return oam.readByte(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        return gamepak.readByte(addr);

    case kRegionGamePak2H:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readByte(addr);
    
    case kRegionSramL:
    case kRegionSramH:
        return readSave(addr);

    default:
        return readUnused(addr);
    }
}

u16 MMU::readHalf(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < 0x4000)
            return bios.readHalf(addr);
        else
            return readUnused(addr);

    case kRegionEwram:
        return ewram.readHalf(addr);

    case kRegionIwram:
        return iwram.readHalf(addr);

    case kRegionIo:
        return io.readHalf(addr);

    case kRegionPalette:
        return palette.readHalf(addr);

    case kRegionVram:
        return vram.readHalf(addr);

    case kRegionOam:
        return oam.readHalf(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        return gamepak.readHalf(addr);

    case kRegionGamePak2H:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readHalf(addr);

    case kRegionSramL:
    case kRegionSramH:
        return readSave(addr) * 0x0101;

    default:
        return readUnused(addr);
    }
}

u32 MMU::readWord(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < 0x4000)
            return bios.readWord(addr);
        else
            return readUnused(addr);

    case kRegionEwram:
        return ewram.readWord(addr);

    case kRegionIwram:
        return iwram.readWord(addr);

    case kRegionIo:
        return io.readWord(addr);

    case kRegionPalette:
        return palette.readWord(addr);

    case kRegionVram:
        return vram.readWord(addr);

    case kRegionOam:
        return oam.readWord(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        return gamepak.readWord(addr);

    case kRegionGamePak2H:
        if (gamepak.save->type == Save::Type::Eeprom
                && (gamepak.size() <= 0x100'0000 || addr >= 0xDFF'FF00))
            return 1;

        return gamepak.readWord(addr);

    case kRegionSramL:
    case kRegionSramH:
        return readSave(addr) * 0x01010101;

    default:
        return readUnused(addr);
    }
}

void MMU::writeByte(u32 addr, u8 byte)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        break;

    case kRegionEwram:
        ewram.writeByte(addr, byte);
        break;

    case kRegionIwram:
        iwram.writeByte(addr, byte);
        break;

    case kRegionIo:
        io.writeByte(addr, byte);
        break;

    case kRegionPalette:
        palette.writeByte(addr, byte);
        break;

    case kRegionVram:
        vram.writeByte(addr, byte);
        break;

    case kRegionOam:
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        break;

    case kRegionSramL:
    case kRegionSramH:
        writeSave(addr, byte);
        break;
    }
}

void MMU::writeHalf(u32 addr, u16 half)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        break;

    case kRegionEwram:
        ewram.writeHalf(addr, half);
        break;

    case kRegionIwram:
        iwram.writeHalf(addr, half);
        break;

    case kRegionIo:
        io.writeHalf(addr, half);
        break;

    case kRegionPalette:
        palette.writeHalf(addr, half);
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
        break;

    case kRegionSramL:
    case kRegionSramH:
        writeSave(addr, half >> ((addr & 0x1) << 3));
        break;
    }
}

void MMU::writeWord(u32 addr, u32 word)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        break;

    case kRegionEwram:
        ewram.writeWord(addr, word);
        break;

    case kRegionIwram:
        iwram.writeWord(addr, word);
        break;

    case kRegionIo:
        io.writeWord(addr, word);
        break;

    case kRegionPalette:
        palette.writeWord(addr, word);
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
        break;

    case kRegionSramL:
    case kRegionSramH:
        writeSave(addr, word >> ((addr & 0x3) << 3));
        break;
    }
}

u32 MMU::readUnused(u32 addr)
{
    u32 value = 0;
    if (core.arm.cpsr.t)
    {
        u32 lsw = core.arm.pipe[1];
        u32 msw = core.arm.pipe[1];

        switch (addr >> 24)
        {
        case kRegionBios:
        case kRegionOam:
            lsw = core.arm.pipe[0];
            break;

        case kRegionIwram:
            if (addr & 0x3)
                lsw = core.arm.pipe[0];
            else
                msw = core.arm.pipe[0];
            break;
        }
        value = (msw << 16) | lsw;
    }
    else
    {
        value = core.arm.pipe[1];
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
