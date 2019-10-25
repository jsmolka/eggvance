#include "dma.h"

#include "arm/arm.h"
#include "mmu/mmu.h"

int DMA::deltas[4] = { 1, -1, 0, 1 };

void DMA::reset()
{
    sad.reset();
    dad.reset();
    control.reset();
}

void DMA::start()
{
    state = State::Running;
    count = control.count;

    if (control.update)
    {
        sad_addr = sad.addr;
        dad_addr = dad.addr;
        control.update = false;
    }
    else if (control.repeat && control.dad_delta == 0b11)
    {
        dad_addr = dad.addr;
    }

    u32 mask = control.word ? ~0x3 : ~0x1;
    sad_addr &= mask;
    dad_addr &= mask;

    sad_delta = (control.word ? 4 : 2) * deltas[control.sad_delta];
    dad_delta = (control.word ? 4 : 2) * deltas[control.dad_delta];
}

void DMA::run(int& cycles)
{
    bool eeprom_w = id == 3 && dad_addr >= 0xD00'0000 && dad_addr < 0xE00'0000;
    bool eeprom_r = id == 3 && sad_addr >= 0xD00'0000 && sad_addr < 0xE00'0000;

    if ((eeprom_r || eeprom_w) && mmu.gamepak.backup->type == Backup::Type::EEPROM)
    {
        if (eeprom_w)
        {
            // Guessing EEPROM size in advance seems to be pretty much impossible.
            // That's why we base the size on the first write (which should happen
            // before the first read).
            if (mmu.gamepak.backup->data.empty())
            {
                switch (control.count)
                {
                    // Bus width 6
                case  9:  // Set address for reading
                case 73:  // Write data to address
                    mmu.gamepak.backup->data.resize(0x0200, 0);
                    break;

                    // Bus width 14
                case 17:  // Set address for reading
                case 81:  // Write data to address
                    mmu.gamepak.backup->data.resize(0x2000, 0);
                    break;

                default:
                    break;
                }
            }
            
            if (!writeEEPROM(cycles))
                return;
        }
        else
        {
            if (!readEEPROM(cycles))
                return;
        }
    }
    else
    {
        while (count-- > 0)
        {
            if (control.word)
            {
                u32 word = mmu.readWord(sad_addr);
                mmu.writeWord(dad_addr, word);
            }
            else
            {
                u32 half = mmu.readHalf(sad_addr);
                mmu.writeHalf(dad_addr, half);
            }

            sad_addr += sad_delta;
            dad_addr += dad_delta;

            cycles -= 2 + 2 * 2;
            if (cycles <= 0)
                return;
        }
    }

    control.enabled = control.repeat;

    if (control.irq)
    {
        static constexpr Interrupt flags[4] = {
            Interrupt::DMA0,
            Interrupt::DMA1,
            Interrupt::DMA2,
            Interrupt::DMA3
        };
        arm.request(flags[id]);
    }
    state = State::Finished;
}

bool DMA::readEEPROM(int& cycles)
{
    while (count-- > 0)
    {
        u8 byte = mmu.gamepak.backup->readByte(sad_addr);
        mmu.writeHalf(dad_addr, byte);

        sad_addr += sad_delta;
        dad_addr += dad_delta;

        cycles -= 2 + 2 * 2;
        if (cycles <= 0)
            return false;
    }
    return true;
}

bool DMA::writeEEPROM(int& cycles)
{
    while (count-- > 0)
    {
        u8 byte = static_cast<u8>(mmu.readHalf(sad_addr));
        mmu.gamepak.backup->writeByte(dad_addr, byte);

        sad_addr += sad_delta;
        dad_addr += dad_delta;

        cycles -= 2 + 2 * 2;
        if (cycles <= 0)
            return false;
    }
    return true;
}
