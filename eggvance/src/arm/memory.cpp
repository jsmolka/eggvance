#include "arm.h"

#include "gamepak/gamepak.h"
#include "mmu/constants.h"
#include "mmu/mmu.h"
#include "timer/timer.h"

u32 Arm::readUnused() const
{
    u32 value = arm.pipe[1];

    if (arm.cpsr.t)
    {
        switch (arm.pc >> 24)
        {
        case kRegionBios:
        case kRegionOam:
            value <<= 16;
            value |= arm.pipe[0];
            break;

        case kRegionInternalWorkRam:
            if (arm.pc & 0x2)
            {
                value <<= 16;
                value |= arm.pipe[0];
            }
            else
            {
                value |= arm.pipe[0] << 16;
            }
            break;

        default:
            value |= value << 16;
            break;
        }
    }
    return value;
}

u8 Arm::readByte(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            clock(1);
            return mmu.bios.readByte(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        clock(1);
        return bit::ror(readUnused(), 8 * (addr & 0x3));

    case kRegionExternalWorkRam:
        clock(3);
        return mmu.ewram.readByte(addr);

    case kRegionInternalWorkRam:
        clock(1);
        return mmu.iwram.readByte(addr);

    case kRegionMmio:
        clock(1);
        return mmu.mmio.readByte(addr);

    case kRegionPaletteRam:
        clock(1);
        return mmu.pram.readByte(addr);

    case kRegionVideoRam:
        clock(1);
        return mmu.vram.readByte(addr);

    case kRegionOam:
        clock(1);
        return mmu.oam.readByte(addr);

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        clock(waitcnt.cyclesHalf(addr, access));
        return gamepak.read<u8>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        clock(waitcnt.cyclesHalf(addr, access));
        return mmu.readSave(addr);

    default:
        clock(1);
        return bit::ror(arm.readUnused(), 8 * (addr & 0x3));
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
            clock(1);
            return mmu.bios.readHalf(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        clock(1);
        return bit::ror(readUnused(), 8 * (addr & 0x2));

    case kRegionExternalWorkRam:
        clock(3);
        return mmu.ewram.readHalf(addr);

    case kRegionInternalWorkRam:
        clock(1);
        return mmu.iwram.readHalf(addr);

    case kRegionMmio:
        clock(1);
        return mmu.mmio.readHalf(addr);

    case kRegionPaletteRam:
        clock(1);
        return mmu.pram.readHalf(addr);

    case kRegionVideoRam:
        clock(1);
        return mmu.vram.readHalf(addr);

    case kRegionOam:
        clock(1);
        return mmu.oam.readHalf(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            clock(waitcnt.cyclesHalf(addr, access));
            return 1;
        }
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        clock(waitcnt.cyclesHalf(addr, access));
        return gamepak.read<u16>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        clock(waitcnt.cyclesHalf(addr, access));
        return mmu.readSave(addr) * 0x0101;

    default:
        clock(1);
        return bit::ror(readUnused(), 8 * (addr & 0x2));
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
            clock(1);
            return mmu.bios.readWord(addr);
        }
        [[fallthrough]];

    case kRegionUnused:
        clock(1);
        return readUnused();

    case kRegionExternalWorkRam:
        clock(6);
        return mmu.ewram.readWord(addr);

    case kRegionInternalWorkRam:
        clock(1);
        return mmu.iwram.readWord(addr);

    case kRegionMmio:
        clock(1);
        return mmu.mmio.readWord(addr);

    case kRegionPaletteRam:
        clock(2);
        return mmu.pram.readWord(addr);

    case kRegionVideoRam:
        clock(2);
        return mmu.vram.readWord(addr);

    case kRegionOam:
        clock(1);
        return mmu.oam.readWord(addr);

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
            return 1;
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        clock(waitcnt.cyclesWord(addr, access));
        return gamepak.read<u32>(addr);

    case kRegionSaveL:
    case kRegionSaveH:
        clock(waitcnt.cyclesWord(addr, access));
        return mmu.readSave(addr) * 0x0101'0101;

    default:
        clock(1);
        return readUnused();
    }
}

void Arm::writeByte(u32 addr, u8 byte, Access access)
{
    pipe.access = Access::NonSequential;
    clock(waitcnt.cyclesHalf(addr, access));
    
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        clock(1);
        break;

    case kRegionExternalWorkRam:
        clock(3);
        mmu.ewram.writeByte(addr, byte);
        break;

    case kRegionInternalWorkRam:
        clock(1);
        mmu.iwram.writeByte(addr, byte);
        break;

    case kRegionMmio:
        clock(1);
        mmu.mmio.writeByte(addr, byte);
        break;

    case kRegionPaletteRam:
        clock(1);
        mmu.pram.writeByte(addr, byte);
        break;

    case kRegionVideoRam:
        clock(1);
        mmu.vram.writeByte(addr, byte);
        break;

    case kRegionOam:
        clock(1);
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
        mmu.writeSave(addr, byte);
        break;

    default:
        clock(1);
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
        clock(1);
        break;

    case kRegionExternalWorkRam:
        clock(3);
        mmu.ewram.writeHalf(addr, half);
        break;

    case kRegionInternalWorkRam:
        clock(1);
        mmu.iwram.writeHalf(addr, half);
        break;

    case kRegionMmio:
        clock(1);
        mmu.mmio.writeHalf(addr, half);
        break;

    case kRegionPaletteRam:
        clock(1);
        mmu.pram.writeHalf(addr, half);
        break;

    case kRegionVideoRam:
        clock(1);
        mmu.vram.writeHalf(addr, half);
        break;

    case kRegionOam:
        clock(1);
        mmu.oam.writeHalf(addr, half);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        clock(waitcnt.cyclesHalf(addr, access));
        gamepak.write<u16>(addr, half);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        clock(waitcnt.cyclesHalf(addr, access));
        mmu.writeSave(addr, half >> (8 * (addr & 0x1)));
        break;

    default:
        clock(1);
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
        clock(1);
        break;

    case kRegionExternalWorkRam:
        clock(6);
        mmu.ewram.writeWord(addr, word);
        break;

    case kRegionInternalWorkRam:
        clock(1);
        mmu.iwram.writeWord(addr, word);
        break;

    case kRegionMmio:
        clock(1);
        mmu.mmio.writeWord(addr, word);
        break;

    case kRegionPaletteRam:
        clock(2);
        mmu.pram.writeWord(addr, word);
        break;

    case kRegionVideoRam:
        clock(2);
        mmu.vram.writeWord(addr, word);
        break;

    case kRegionOam:
        clock(1);
        mmu.oam.writeWord(addr, word);
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        clock(waitcnt.cyclesWord(addr, access));
        gamepak.write<u32>(addr, word);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        clock(waitcnt.cyclesWord(addr, access));
        mmu.writeSave(addr, word >> (8 * (addr & 0x3)));
        break;

    default:
        clock(1);
        break;
    }
}

u32 Arm::readWordRotate(u32 addr, Access access)
{
    u32 value = readWord(addr, access);
    return bit::ror(value, 8 * (addr & 0x3));
}

u32 Arm::readHalfRotate(u32 addr, Access access)
{
    u32 value = readHalf(addr, access);
    return bit::ror(value, 8 * (addr & 0x1));
}

u32 Arm::readHalfSignEx(u32 addr, Access access)
{
    if (addr & 0x1)
    {
        u32 value = readByte(addr, access);
        return bit::signEx<8>(value);
    }
    else
    {
        u32 value = readHalf(addr, access);
        return bit::signEx<16>(value);
    }
}
