#include "dma3.h"

#include <fmt/printf.h>

#include "mmu/mmu.h"

DMA::Status DMA3::emulate(int& cycles)
{
    bool eeprom_w = regs.dad >= 0xD00'0000 && regs.dad < 0xE00'0000;
    bool eeprom_r = regs.sad >= 0xD00'0000 && regs.sad < 0xE00'0000;

    if ((eeprom_r || eeprom_w) && mmu.gamepak->save->type == Save::Type::EEPROM)
    {
        if (eeprom_w)
        {
            // Guessing EEPROM size in advance seems to be pretty much impossible.
            // That's why we base the size on the first write (which should happen
            // before the first read).
            if (mmu.gamepak->save->data.empty())
            {
                switch (control.count)
                {
                // Bus width 6
                case  9:  // Set address for reading
                case 73:  // Write data to address
                    mmu.gamepak->save->data.resize(0x0200, 0);
                    break;

                // Bus width 14
                case 17:  // Set address for reading
                case 81:  // Write data to address
                    mmu.gamepak->save->data.resize(0x2000, 0);
                    break;

                default:
                    fmt::printf("DMA: Unexpected EEPROM write count %d\n", control.count);
                    break;
                }
            }

            if (!writeEEPROM(cycles))
                return RUNNING;
        }
        else
        {
            if (!readEEPROM(cycles))
                return RUNNING;
        }
    }
    else
    {
        if (!transfer(cycles))
            return RUNNING;
    }

    finish();

    return DISABLED;
}

bool DMA3::readEEPROM(int& cycles)
{
    while (count-- > 0)
    {
        u8 byte = mmu.gamepak->save->readByte(regs.sad);
        mmu.writeHalf(regs.dad, byte);

        regs.sad += sad_delta;
        regs.dad += dad_delta;

        cycles -= 2 + 2 * sequential;
        if (cycles <= 0)
            return false;
    }
    return true;
}

bool DMA3::writeEEPROM(int& cycles)
{
    while (count-- > 0)
    {
        u8 byte = static_cast<u8>(mmu.readHalf(regs.sad));
        mmu.gamepak->save->writeByte(regs.dad, byte);

        regs.sad += sad_delta;
        regs.dad += dad_delta;

        cycles -= 2 + 2 * sequential;
        if (cycles <= 0)
            return false;
    }
    return true;
}
