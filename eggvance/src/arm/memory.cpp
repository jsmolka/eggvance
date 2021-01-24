#include "arm.h"

#include "gamepak/gamepak.h"
#include "ppu/ppu.h"

enum Region
{
    kRegionBios,
    kRegionUnused,
    kRegionExternalWorkRam,
    kRegionInternalWorkRam,
    kRegionIo,
    kRegionPaletteRam,
    kRegionVideoRam,
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

u8 Arm::readByte(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readByte(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x3));

    case kRegionExternalWorkRam:
        tickRam(3);
        return ewram.readByte(addr);

    case kRegionInternalWorkRam:
        tickRam(1);
        return iwram.readByte(addr);

    case kRegionIo:
        tickRam(1);
        return readIo(addr);

    case kRegionPaletteRam:
        tickRam(1);
        return ppu.pram.readByte(addr);

    case kRegionVideoRam:
        tickRam(1);
        return ppu.vram.readByte(addr);

    case kRegionOam:
        tickRam(1);
        return ppu.oam.readByte(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        return gamepak.read<u8>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        return gamepak.readSave(addr);

    default:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x3));
    }
}

u16 Arm::readHalf(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readHalf(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x2));

    case kRegionExternalWorkRam:
        tickRam(3);
        return ewram.readHalf(addr);

    case kRegionInternalWorkRam:
        tickRam(1);
        return iwram.readHalf(addr);

    case kRegionIo:
        tickRam(1);
        addr &= ~0x1;
        return readIo(addr + 0) << 0
             | readIo(addr + 1) << 8;

    case kRegionPaletteRam:
        tickRam(1);
        return ppu.pram.readHalf(addr);

    case kRegionVideoRam:
        tickRam(1);
        return ppu.vram.readHalf(addr);

    case kRegionOam:
        tickRam(1);
        return ppu.oam.readHalf(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            tickRom(addr, waitcnt.cyclesHalf(addr, access));
            return 1;
        }
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        return gamepak.read<u16>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        return gamepak.readSave(addr) * 0x0101;

    default:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x2));
    }
}

u32 Arm::readWord(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readWord(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        tickRam(1);
        return readUnused();

    case kRegionExternalWorkRam:
        tickRam(6);
        return ewram.readWord(addr);

    case kRegionInternalWorkRam:
        tickRam(1);
        return iwram.readWord(addr);

    case kRegionIo:
        tickRam(1);
        addr &= ~0x3;
        return readIo(addr + 0) <<  0
             | readIo(addr + 1) <<  8
             | readIo(addr + 2) << 16
             | readIo(addr + 3) << 24;

    case kRegionPaletteRam:
        tickRam(2);
        return ppu.pram.readWord(addr);

    case kRegionVideoRam:
        tickRam(2);
        return ppu.vram.readWord(addr);

    case kRegionOam:
        tickRam(1);
        return ppu.oam.readWord(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            tickRom(addr, waitcnt.cyclesWord(addr, access));
            return 1;
        }
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        tickRom(addr, waitcnt.cyclesWord(addr, access));
        return gamepak.read<u32>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesWord(addr, access));
        return gamepak.readSave(addr) * 0x0101'0101;

    default:
        tickRam(1);
        return readUnused();
    }
}

void Arm::writeByte(u32 addr, u8 byte, Access access)
{
    pipe.access = Access::NonSequential;

    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        tickRam(1);
        break;

    case kRegionExternalWorkRam:
        tickRam(3);
        ewram.writeByte(addr, byte);
        break;

    case kRegionInternalWorkRam:
        tickRam(1);
        iwram.writeByte(addr, byte);
        break;

    case kRegionIo:
        tickRam(1);
        writeIo(addr, byte);
        break;

    case kRegionPaletteRam:
        tickRam(1);
        ppu.pram.writeByte(addr, byte);
        break;

    case kRegionVideoRam:
        tickRam(1);
        ppu.vram.writeByte(addr, byte);
        break;

    case kRegionOam:
        tickRam(1);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        gamepak.write<u8>(addr, byte);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        gamepak.writeSave(addr, byte);
        break;

    default:
        tickRam(1);
        break;
    }
}

void Arm::writeHalf(u32 addr, u16 half, Access access)
{
    pipe.access = Access::NonSequential;
    
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        tickRam(1);
        break;

    case kRegionExternalWorkRam:
        tickRam(3);
        ewram.writeHalf(addr, half);
        break;

    case kRegionInternalWorkRam:
        tickRam(1);
        iwram.writeHalf(addr, half);
        break;

    case kRegionIo:
        tickRam(1);
        addr &= ~0x1;
        writeIo(addr + 0, bit::seq<0, 8>(half));
        writeIo(addr + 1, bit::seq<8, 8>(half));
        break;

    case kRegionPaletteRam:
        tickRam(1);
        ppu.pram.writeHalf(addr, half);
        break;

    case kRegionVideoRam:
        tickRam(1);
        ppu.vram.writeHalf(addr, half);
        break;

    case kRegionOam:
        tickRam(1);
        ppu.oam.writeHalf(addr, half);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        gamepak.write<u16>(addr, half);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesHalf(addr, access));
        gamepak.writeSave(addr, half >> (8 * (addr & 0x1)));
        break;

    default:
        tickRam(1);
        break;
    }
}

void Arm::writeWord(u32 addr, u32 word, Access access)
{
    pipe.access = Access::NonSequential;
    
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        tickRam(1);
        break;

    case kRegionExternalWorkRam:
        tickRam(6);
        ewram.writeWord(addr, word);
        break;

    case kRegionInternalWorkRam:
        tickRam(1);
        iwram.writeWord(addr, word);
        break;

    case kRegionIo:
        tickRam(1);
        addr &= ~0x3;
        writeIo(addr + 0, bit::seq< 0, 8>(word));
        writeIo(addr + 1, bit::seq< 8, 8>(word));
        writeIo(addr + 2, bit::seq<16, 8>(word));
        writeIo(addr + 3, bit::seq<24, 8>(word));
        break;

    case kRegionPaletteRam:
        tickRam(2);
        ppu.pram.writeWord(addr, word);
        break;

    case kRegionVideoRam:
        tickRam(2);
        ppu.vram.writeWord(addr, word);
        break;

    case kRegionOam:
        tickRam(1);
        ppu.oam.writeWord(addr, word);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        tickRom(addr, waitcnt.cyclesWord(addr, access));
        gamepak.write<u32>(addr, word);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        tickRom(addr, waitcnt.cyclesWord(addr, access));
        gamepak.writeSave(addr, word >> (8 * (addr & 0x3)));
        break;

    default:
        tickRam(1);
        break;
    }
}

u32 Arm::readUnused() const
{
    if (cpsr.t == 0)
        return pipe[1];

    switch (pc >> 24)
    {
    case kRegionBios:
    case kRegionOam:
        return pipe[1] << 16 | pipe[0];

    case kRegionInternalWorkRam:
        return pc & 0x2
            ? pipe[1] << 16 | pipe[0]
            : pipe[0] << 16 | pipe[1];

    default:
        return pipe[1] << 16 | pipe[1];
    }
}

u32 Arm::readHalfRotate(u32 addr, Access access)
{
    u32 value = readHalf(addr, access);
    return bit::ror(value, 8 * (addr & 0x1));
}

u32 Arm::readWordRotate(u32 addr, Access access)
{
    u32 value = readWord(addr, access);
    return bit::ror(value, 8 * (addr & 0x3));
}

u32 Arm::readByteSignEx(u32 addr, Access access)
{
    u32 value = readByte(addr, access);
    return bit::signEx<8>(value);
}

u32 Arm::readHalfSignEx(u32 addr, Access access)
{
    u32 value = readHalf(addr, access);
    value = bit::signEx<16>(value);
    return bit::sar(value, 8 * (addr & 0x1));
}
