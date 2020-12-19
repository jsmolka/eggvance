#include "mmu.h"

#include "constants.h"
#include "arm/arm.h"
#include "gamepak/gamepak.h"

u8 Mmu::readByte(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readByte(addr);
        [[fallthrough]];

    case kRegionUnused:
        return bit::ror(arm.readUnused(), 8 * (addr & 0x3));

    case kRegionExternalWorkRam:
        return ewram.readByte(addr);

    case kRegionInternalWorkRam:
        return iwram.readByte(addr);

    case kRegionMmio:
        return mmio.readByte(addr);

    case kRegionPaletteRam:
        return pram.readByte(addr);

    case kRegionVideoRam:
        return vram.readByte(addr);

    case kRegionOam:
        return oam.readByte(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        return gamepak.read<u8>(addr);
    
    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr);

    default:
        return bit::ror(arm.readUnused(), 8 * (addr & 0x3));
    }
}

u16 Mmu::readHalf(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readHalf(addr);
        [[fallthrough]];

    case kRegionUnused:
        return bit::ror(arm.readUnused(), 8 * (addr & 0x2));

    case kRegionExternalWorkRam:
        return ewram.readHalf(addr);

    case kRegionInternalWorkRam:
        return iwram.readHalf(addr);

    case kRegionMmio:
        return mmio.readHalf(addr);

    case kRegionPaletteRam:
        return pram.readHalf(addr);

    case kRegionVideoRam:
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
        return gamepak.read<u16>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr) * 0x0101;

    default:
        return bit::ror(arm.readUnused(), 8 * (addr & 0x2));
    }
}

u32 Mmu::readWord(u32 addr)
{
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
            return bios.readWord(addr);
        [[fallthrough]];

    case kRegionUnused:
        return arm.readUnused();

    case kRegionExternalWorkRam:
        return ewram.readWord(addr);

    case kRegionInternalWorkRam:
        return iwram.readWord(addr);

    case kRegionMmio:
        return mmio.readWord(addr);

    case kRegionPaletteRam:
        return pram.readWord(addr);

    case kRegionVideoRam:
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
        return gamepak.read<u32>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        return readSave(addr) * 0x0101'0101;

    default:
        return arm.readUnused();
    }
}

void Mmu::writeByte(u32 addr, u8 byte)
{
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        break;

    case kRegionExternalWorkRam:
        ewram.writeByte(addr, byte);
        break;

    case kRegionInternalWorkRam:
        iwram.writeByte(addr, byte);
        break;

    case kRegionMmio:
        mmio.writeByte(addr, byte);
        break;

    case kRegionPaletteRam:
        pram.writeByte(addr, byte);
        break;

    case kRegionVideoRam:
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
        gamepak.write<u8>(addr, byte);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, byte);
        break;
    }
}

void Mmu::writeHalf(u32 addr, u16 half)
{
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        break;

    case kRegionExternalWorkRam:
        ewram.writeHalf(addr, half);
        break;

    case kRegionInternalWorkRam:
        iwram.writeHalf(addr, half);
        break;

    case kRegionMmio:
        mmio.writeHalf(addr, half);
        break;

    case kRegionPaletteRam:
        pram.writeHalf(addr, half);
        break;

    case kRegionVideoRam:
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
        gamepak.write<u16>(addr, half);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, half >> (8 * (addr & 0x1)));
        break;
    }
}

void Mmu::writeWord(u32 addr, u32 word)
{
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        break;

    case kRegionExternalWorkRam:
        ewram.writeWord(addr, word);
        break;

    case kRegionInternalWorkRam:
        iwram.writeWord(addr, word);
        break;

    case kRegionMmio:
        mmio.writeWord(addr, word);
        break;

    case kRegionPaletteRam:
        pram.writeWord(addr, word);
        break;

    case kRegionVideoRam:
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
        gamepak.write<u32>(addr, word);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        writeSave(addr, word >> (8 * (addr & 0x3)));
        break;
    }
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
    }
}
