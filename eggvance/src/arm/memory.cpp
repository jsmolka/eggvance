#include "arm.h"

#include "gamepak/gamepak.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"

enum class Region
{
    Bios,
    Unused,
    ExternalWorkRam,
    InternalWorkRam,
    Io,
    PaletteRam,
    VideoRam,
    Oam,
    GamePak0L,
    GamePak0H,
    GamePak1L,
    GamePak1H,
    GamePak2L,
    GamePak2H,
    SaveL,
    SaveH
};

u8 Arm::readByte(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (Region(addr >> 24))
    {
    case Region::Bios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readByte(addr);
        }
        [[fallthrough]];

    case Region::Unused:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x3));

    case Region::ExternalWorkRam:
        tickRam(3);
        return ewram.readByte(addr);

    case Region::InternalWorkRam:
        tickRam(1);
        return iwram.readByte(addr);

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFF;
        return readIo(addr);

    case Region::PaletteRam:
        tickRam(1);
        return ppu.pram.readByte(addr);

    case Region::VideoRam:
        tickRam(1);
        return ppu.vram.readByte(addr);

    case Region::Oam:
        tickRam(1);
        return ppu.oam.readByte(addr);

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
    case Region::GamePak2H:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        return gamepak.read<u8>(addr);

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        return gamepak.readSave(addr);

    default:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x3));
    }
}

u16 Arm::readHalf(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (Region(addr >> 24))
    {
    case Region::Bios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readHalf(addr);
        }
        [[fallthrough]];

    case Region::Unused:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x2));

    case Region::ExternalWorkRam:
        tickRam(3);
        return ewram.readHalf(addr);

    case Region::InternalWorkRam:
        tickRam(1);
        return iwram.readHalf(addr);

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFE;
        return readIo(addr + 0) << 0
             | readIo(addr + 1) << 8;

    case Region::PaletteRam:
        tickRam(1);
        return ppu.pram.readHalf(addr);

    case Region::VideoRam:
        tickRam(1);
        return ppu.vram.readHalf(addr);

    case Region::Oam:
        tickRam(1);
        return ppu.oam.readHalf(addr);

    case Region::GamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            tickRom(addr, waitcnt.waitHalf(addr, access));
            return 1;
        }
        [[fallthrough]];

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        return gamepak.read<u16>(addr);

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        return gamepak.readSave(addr) * 0x0101;

    default:
        tickRam(1);
        return readUnused() >> (8 * (addr & 0x2));
    }
}

u32 Arm::readWord(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (Region(addr >> 24))
    {
    case Region::Bios:
        if (addr < Bios::kSize)
        {
            tickRam(1);
            return bios.readWord(addr);
        }
        [[fallthrough]];

    case Region::Unused:
        tickRam(1);
        return readUnused();

    case Region::ExternalWorkRam:
        tickRam(6);
        return ewram.readWord(addr);

    case Region::InternalWorkRam:
        tickRam(1);
        return iwram.readWord(addr);

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFC;
        return readIo(addr + 0) <<  0
             | readIo(addr + 1) <<  8
             | readIo(addr + 2) << 16
             | readIo(addr + 3) << 24;

    case Region::PaletteRam:
        tickRam(2);
        return ppu.pram.readWord(addr);

    case Region::VideoRam:
        tickRam(2);
        return ppu.vram.readWord(addr);

    case Region::Oam:
        tickRam(1);
        return ppu.oam.readWord(addr);

    case Region::GamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            tickRom(addr, waitcnt.waitWord(addr, access));
            return 1;
        }
        [[fallthrough]];

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
        tickRom(addr, waitcnt.waitWord(addr, access));
        return gamepak.read<u32>(addr);

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitWord(addr, access));
        return gamepak.readSave(addr) * 0x0101'0101;

    default:
        tickRam(1);
        return readUnused();
    }
}

void Arm::writeByte(u32 addr, u8 byte, Access access)
{
    pipe.access = Access::NonSequential;

    switch (Region(addr >> 24))
    {
    case Region::Bios:
    case Region::Unused:
        tickRam(1);
        break;

    case Region::ExternalWorkRam:
        tickRam(3);
        ewram.writeByte(addr, byte);
        break;

    case Region::InternalWorkRam:
        tickRam(1);
        iwram.writeByte(addr, byte);
        break;

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFF;
        writeIo(addr, byte);
        if ((addr & ~0x1) == Io::KeyControl)
            keypad.checkInterrupt();
        break;

    case Region::PaletteRam:
        tickRam(1);
        ppu.pram.writeByte(addr, byte);
        break;

    case Region::VideoRam:
        tickRam(1);
        ppu.vram.writeByte(addr, byte);
        break;

    case Region::Oam:
        tickRam(1);
        break;

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
    case Region::GamePak2H:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        gamepak.write<u8>(addr, byte);
        break;

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitHalf(addr, access));
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
    
    switch (Region(addr >> 24))
    {
    case Region::Bios:
    case Region::Unused:
        tickRam(1);
        break;

    case Region::ExternalWorkRam:
        tickRam(3);
        ewram.writeHalf(addr, half);
        break;

    case Region::InternalWorkRam:
        tickRam(1);
        iwram.writeHalf(addr, half);
        break;

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFE;
        writeIo(addr + 0, bit::seq<0, 8>(half));
        writeIo(addr + 1, bit::seq<8, 8>(half));
        if (addr == Io::KeyControl)
            keypad.checkInterrupt();
        break;

    case Region::PaletteRam:
        tickRam(1);
        ppu.pram.writeHalf(addr, half);
        break;

    case Region::VideoRam:
        tickRam(1);
        ppu.vram.writeHalf(addr, half);
        break;

    case Region::Oam:
        tickRam(1);
        ppu.oam.writeHalf(addr, half);
        break;

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
    case Region::GamePak2H:
        tickRom(addr, waitcnt.waitHalf(addr, access));
        gamepak.write<u16>(addr, half);
        break;

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitHalf(addr, access));
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
    
    switch (Region(addr >> 24))
    {
    case Region::Bios:
    case Region::Unused:
        tickRam(1);
        break;

    case Region::ExternalWorkRam:
        tickRam(6);
        ewram.writeWord(addr, word);
        break;

    case Region::InternalWorkRam:
        tickRam(1);
        iwram.writeWord(addr, word);
        break;

    case Region::Io:
        tickRam(1);
        addr &= 0x3FF'FFFC;
        writeIo(addr + 0, bit::seq< 0, 8>(word));
        writeIo(addr + 1, bit::seq< 8, 8>(word));
        writeIo(addr + 2, bit::seq<16, 8>(word));
        writeIo(addr + 3, bit::seq<24, 8>(word));
        if (addr == Io::KeyInput)
            keypad.checkInterrupt();
        break;

    case Region::PaletteRam:
        tickRam(2);
        ppu.pram.writeWord(addr, word);
        break;

    case Region::VideoRam:
        tickRam(2);
        ppu.vram.writeWord(addr, word);
        break;

    case Region::Oam:
        tickRam(1);
        ppu.oam.writeWord(addr, word);
        break;

    case Region::GamePak0L:
    case Region::GamePak0H:
    case Region::GamePak1L:
    case Region::GamePak1H:
    case Region::GamePak2L:
    case Region::GamePak2H:
        tickRom(addr, waitcnt.waitWord(addr, access));
        gamepak.write<u32>(addr, word);
        break;

    case Region::SaveL:
    case Region::SaveH:
        tickRom(addr, waitcnt.waitWord(addr, access));
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

    switch (Region(pc >> 24))
    {
    case Region::Bios:
    case Region::Oam:
    case Region::InternalWorkRam:
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
