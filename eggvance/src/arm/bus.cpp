#include "arm.h"

// Todo: proper mmio
// Todo: implement all read types

#include "common/utility.h"
#include "memory/memmap.h"
#include "memory/memory.h"

u8 ARM::readByte(u32 addr)
{
    int page = addr >> 24;

    switch (page)
    {
    case PAGE_BIOS:
        //if (addr < 0x4000)
        //    return bios->readByte(addr);
        //else
            return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        return mem->ewram.readByte(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        return mem->iwram.readByte(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
            return mem->io.readByte(addr);
        }
        return 0;

    case PAGE_PALETTE:
        addr &= 0x3FF;
        return mem->pram.readByte(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFF;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        return mem->vram.readByte(addr);

    case PAGE_OAM:
        addr &= 0x3FF;
        return mem->oam.readByte(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFF;
        return mem->cartridge.readByte(addr);

    //case PAGE_GAMEPAK_2+1:
    //    if (gamepak->save->type == Save::Type::EEPROM)
    //    {
    //        if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
    //            return 1;
    //    }
    //    addr &= 0x1FF'FFFF;
    //    return gamepak->readByte(addr);

    //case PAGE_GAMEPAK_SRAM:
    //    switch (gamepak->save->type)
    //    {
    //    case Save::Type::SRAM:
    //        addr &= 0x7FFF;
    //        return gamepak->save->readByte(addr);

    //    case Save::Type::FLASH64:
    //    case Save::Type::FLASH128:
    //        addr &= 0xFFFF;
    //        return gamepak->save->readByte(addr);
    //    }
    //    return 0;

    default:
        //if (gamepak->save->type == Save::Type::SRAM)
        //{
        //    addr &= 0x7FFF;
        //    return gamepak->save->readByte(addr);
        //}
        return 0;
    }
    return 0;
}

u16 ARM::readHalf(u32 addr)
{
    int page = addr >> 24;

    switch (page)
    {
    case PAGE_BIOS:
        //if (addr < 0x4000)
        //{
        //    addr &= 0x3FFE;
        //    return bios->readHalf(addr);
        //}
        return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFE;
        return mem->ewram.readHalf(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFE;
        return mem->iwram.readHalf(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            return mem->io.readHalf(addr);
        }
        return 0;

    case PAGE_PALETTE:
        addr &= 0x3FE;
        return mem->pram.readHalf(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFE;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        return mem->vram.readHalf(addr);

    case PAGE_OAM:
        addr &= 0x3FE;
        return mem->oam.readHalf(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFE;
        return mem->cartridge.readHalf(addr);

    //case PAGE_GAMEPAK_2+1:
    //    if (gamepak->save->type == Save::Type::EEPROM)
    //    {
    //        if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
    //            return 1;
    //    }
    //    addr &= 0x1FF'FFFE;
    //    return gamepak->readHalf(addr);

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        return 0;
    }
    return 0;

}

u32 ARM::readWord(u32 addr)
{
    int page = addr >> 24;

    switch (page)
    {
    case PAGE_BIOS:
        //if (addr < 0x4000)
        //{
        //    addr &= 0x3FFC;
        //    return bios->readWord(addr);
        //}
        return 0;

    case PAGE_BIOS+1:
        return 0;

    case PAGE_EWRAM:
        addr &= 0x3'FFFC;
        return mem->ewram.readWord(addr);

    case PAGE_IWRAM:
        addr &= 0x7FFC;
        return mem->iwram.readWord(addr);

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            return mem->io.readWord(addr);
        }
        return 0;

    case PAGE_PALETTE:
        addr &= 0x3FC;
        return mem->pram.readWord(addr);

    case PAGE_VRAM:
        addr &= 0x1'FFFC;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        return mem->vram.readWord(addr);

    case PAGE_OAM:
        addr &= 0x3FC;
        return mem->oam.readWord(addr);

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
        addr &= 0x1FF'FFFC;
        return mem->cartridge.readWord(addr);

    //case PAGE_GAMEPAK_2+1:
    //    if (gamepak->save->type == Save::Type::EEPROM)
    //    {
    //        if (gamepak->size() <= 0x100'0000 || (addr >= 0xDFF'FF00 && addr < 0xE00'0000))
    //            return 1;
    //    }
    //    addr &= 0x1FF'FFFC;
    //    return gamepak->readWord(addr);

    case PAGE_GAMEPAK_SRAM:
    case PAGE_UNUSED:
        return 0;
    }
    return 0;
}

void ARM::writeByte(u32 addr, u8 byte)
{
    int page = addr >> 24;

    switch (page)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFF;
        mem->ewram.writeByte(addr, byte);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFF;
        mem->iwram.writeByte(addr, byte);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FF;
            mem->io.writeByte(addr, byte);

            //switch (addr)
            //{
            //case REG_DMA0CNT_H+1:
            //case REG_DMA1CNT_H+1:
            //case REG_DMA2CNT_H+1:
            //case REG_DMA3CNT_H+1:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        addr &= 0x3FE;
        mem->pram.writeHalf(addr, byte * 0x0101);
        break;

    case PAGE_VRAM:
        //addr &= 0x1'FFFE;
        //if (mmio.dispcnt.mode < 3
        //    ? addr < 0x1'0000  
        //    : addr < 0x1'4000)
        //    vram.writeHalf(addr, byte * 0x0101);
        break;

    case PAGE_OAM:
        break;

    case PAGE_GAMEPAK_0:
    case PAGE_GAMEPAK_0+1:
    case PAGE_GAMEPAK_1:
    case PAGE_GAMEPAK_1+1:
    case PAGE_GAMEPAK_2:
    case PAGE_GAMEPAK_2+1:
        break;

    //case PAGE_GAMEPAK_SRAM:
    //    switch (gamepak->save->type)
    //    {
    //    case Save::Type::SRAM:
    //        addr &= 0x7FFF;
    //        gamepak->save->writeByte(addr, byte);
    //        break;

    //    case Save::Type::FLASH64:
    //    case Save::Type::FLASH128:
    //        addr &= 0xFFFF;
    //        gamepak->save->writeByte(addr, byte);
    //        break;
    //    }
    //    break;

    default:
        //if (gamepak->save->type == Save::Type::SRAM)
        //{
        //    addr &= 0x7FFF;
        //    gamepak->save->writeByte(addr, byte);
        //}
        break;
    }
}

void ARM::writeHalf(u32 addr, u16 half)
{
    int page = addr >> 24;

    switch (page)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFE;
        mem->ewram.writeHalf(addr, half);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFE;
        mem->iwram.writeHalf(addr, half);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FE;
            mem->io.writeHalf(addr, half);

            //switch (addr)
            //{
            //case REG_DMA0CNT_H:
            //case REG_DMA1CNT_H:
            //case REG_DMA2CNT_H:
            //case REG_DMA3CNT_H:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        addr &= 0x3FE;
        mem->pram.writeHalf(addr, half);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFE;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        mem->vram.writeHalf(addr, half);
        break;

    case PAGE_OAM:
        addr &= 0x3FE;
        //writeOAM(addr, half);
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

void ARM::writeWord(u32 addr, u32 word)
{
    switch (addr >> 24)
    {
    case PAGE_BIOS:
    case PAGE_BIOS+1:
        break;

    case PAGE_EWRAM:
        addr &= 0x3'FFFC;
        mem->ewram.writeWord(addr, word);
        break;

    case PAGE_IWRAM:
        addr &= 0x7FFC;
        mem->iwram.writeWord(addr, word);
        break;

    case PAGE_IO:
        if (addr < (MAP_IO + 0x400))
        {
            addr &= 0x3FC;
            mem->io.writeWord(addr, word);

            //switch (addr)
            //{
            //case REG_DMA0CNT_L:
            //case REG_DMA1CNT_L:
            //case REG_DMA2CNT_L:
            //case REG_DMA3CNT_L:
            //    signalDMA(DMA::Timing::IMMEDIATE);
            //    break;
            //}
        }
        break;

    case PAGE_PALETTE:
        addr &= 0x3FC;
        mem->pram.writeWord(addr, word);
        break;

    case PAGE_VRAM:
        addr &= 0x1'FFFC;
        if (addr >= 0x1'8000)
            addr -= 0x8000;
        mem->vram.writeWord(addr, word);
        break;

    case PAGE_OAM:
        addr &= 0x3FC;
        //writeOAM(addr + 0, (word >>  0) & 0xFFFF);
        //writeOAM(addr + 2, (word >> 16) & 0xFFFF);
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

u32 ARM::readWordRotated(u32 addr)
{
    u32 value = readWord(addr);
    if (misalignedWord(addr))
    {
        int rotation = (addr & 0x3) << 3;
        value = ror(value, rotation, true);
    }
    return value;
}

u32 ARM::readHalfRotated(u32 addr)
{
    u32 value = readHalf(addr);
    if (misalignedHalf(addr))
    {
        value = ror(value, 8, true);
    }
    return value;
}

u32 ARM::readHalfSigned(u32 addr)
{
    u32 value = 0;
    if (misalignedHalf(addr))
    {
        value = readByte(addr);
        value = signExtend<8>(value);
    }
    else
    {
        value = readHalf(addr);
        value = signExtend<16>(value);
    }
    return value;
}
