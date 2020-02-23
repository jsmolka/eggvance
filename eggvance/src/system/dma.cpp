#include "dma.h"

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "system/irqhandler.h"

static constexpr int count_limits[4] = { 
    0x04000, 
    0x04000, 
    0x04000, 
    0x10000 
};

DMA::DMA(int id)
    : count(count_limits[id])
{
    this->id = id;
}

void DMA::reset()
{
    remaining = 0;
    sad_addr  = 0;
    dad_addr  = 0;
    sad_delta = 0;
    dad_delta = 0;
    cycles_s  = 0;
    cycles_n  = 0;
    running   = false;
    transfer  = nullptr;

    control.reset();
    count.reset();
    sad.reset();
    dad.reset();
}

void DMA::start()
{
    running   = true;
    remaining = count.count();

    if (control.reload)
    {
        sad_addr = sad;
        dad_addr = dad;
        control.reload = false;
    }
    else if (control.repeat && control.dad_delta == 0b11)
    {
        dad_addr = dad;
    }

    int size = control.word ? 4 : 2;

    sad_addr &= ~(size - 1);
    dad_addr &= ~(size - 1);

    static constexpr int deltas[4] = { 1, -1, 0, 1 };

    sad_delta = size * deltas[control.sad_delta];
    dad_delta = size * deltas[control.dad_delta];

    updateCycles();
    updateTransfer();
}

void DMA::run(int& cycles)
{
    while (remaining-- > 0)
    {
        (this->*transfer)();

        sad_addr += sad_delta;
        dad_addr += dad_delta;

        if (remaining == 0)
            cycles -= cycles_n;
        else
            cycles -= cycles_s;

        if (cycles <= 0)
            return;
    }

    control.enabled = control.repeat;

    if (control.irq)
        irqh.request(
            static_cast<Irq>(
                static_cast<uint>(Irq::Dma) << id));

    running = false;
}

bool DMA::inEEPROM(u32 addr)
{
    return addr >= 0xD00'0000 && addr < 0xE00'0000;
}

bool DMA::inGamePak(u32 addr)
{
    return addr >= 0x800'0000 && addr < 0xE00'0000;
}

void DMA::updateCycles()
{
    if (inGamePak(sad_addr) && inGamePak(dad_addr))
    {
        cycles_s = 4;
        cycles_n = 4;
    }
    else
    {
        cycles_s = 2;
        cycles_n = 2;
    }

    if (control.word)
    {
        cycles_s += arm.io.waitcnt.cyclesWord(sad_addr, true);
        cycles_s += arm.io.waitcnt.cyclesWord(dad_addr, true);
        cycles_n += arm.io.waitcnt.cyclesWord(sad_addr, false);
        cycles_n += arm.io.waitcnt.cyclesWord(dad_addr, false);
    }
    else
    {
        cycles_s += arm.io.waitcnt.cyclesHalf(sad_addr, true);
        cycles_s += arm.io.waitcnt.cyclesHalf(dad_addr, true);
        cycles_n += arm.io.waitcnt.cyclesHalf(sad_addr, false);
        cycles_n += arm.io.waitcnt.cyclesHalf(dad_addr, false);
    }
}

void DMA::updateTransfer()
{
    bool eeprom_w = id == 3 && inEEPROM(dad_addr);
    bool eeprom_r = id == 3 && inEEPROM(sad_addr);

    if ((eeprom_r || eeprom_w) && mmu.gamepak.backup->type == Backup::Type::EEPROM)
    {
        initEEPROM();

        if (eeprom_w)
            transfer = &DMA::writeEEPROM;
        else
            transfer = &DMA::readEEPROM;
    }
    else
    {
        if (control.word)
            transfer = &DMA::transferWord;
        else
            transfer = &DMA::transferHalf;
    }
}

void DMA::transferHalf()
{
    u32 half = mmu.readHalf(sad_addr);
    mmu.writeHalf(dad_addr, half);
}

void DMA::transferWord()
{
    u32 word = mmu.readWord(sad_addr);
    mmu.writeWord(dad_addr, word);
}

void DMA::initEEPROM()
{
    // Guessing EEPROM size in advance seems to be pretty much impossible.
    // That's why we base the size on the first write (which should happen
    // before the first read).
    if (mmu.gamepak.backup->data.empty())
    {
        switch (remaining)
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
        }
    }
}

void DMA::readEEPROM()
{
    u8 byte = mmu.gamepak.backup->readByte(sad_addr);
    mmu.writeHalf(dad_addr, byte);
}

void DMA::writeEEPROM()
{
    u8 byte = static_cast<u8>(mmu.readHalf(sad_addr));
    mmu.gamepak.backup->writeByte(dad_addr, byte);
}
