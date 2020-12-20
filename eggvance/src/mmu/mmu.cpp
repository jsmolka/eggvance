#include "mmu.h"

#include "constants.h"
#include "arm/arm.h"
#include "gamepak/gamepak.h"

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
