#include "dma.h"

#include "arm/arm.h"
#include "interrupt/irqhandler.h"
#include "mmu/mmu.h"

static constexpr int deltas[4] = { 1, -1, 0, 1 };

enum AddressControl
{
    kAddressControlIncrement,
    kAddressControlDecrement,
    kAddressControlFixed,
    kAddressControlReload
};

DMA::DMA(uint id)
{
    this->id = id;
}

void DMA::start()
{
    running   = true;
    remaining = io.count.count(id);

    if (io.control.reload)
    {
        sad = io.sad;
        dad = io.dad;
        io.control.reload = false;
    }
    else if (io.control.repeat && io.control.dadcnt == kAddressControlReload)
    {
        dad = io.dad;
    }

    uint size = 2 << io.control.word;

    sad &= ~(size - 1);
    dad &= ~(size - 1);

    sad_delta = size * deltas[io.control.sadcnt];
    dad_delta = size * deltas[io.control.dadcnt];

    updateCycles();
    updateTransfer();
}

void DMA::run(int& cycles)
{
    while (remaining-- > 0)
    {
        transfer();

        sad += sad_delta;
        dad += dad_delta;

        if (remaining == 0)
            cycles -= cycles_n;
        else
            cycles -= cycles_s;

        if (cycles <= 0)
            return;
    }

    io.control.enable = io.control.repeat;

    if (io.control.irq)
        irqh.request(
            static_cast<IRQ>(
                static_cast<uint>(IRQ::Dma) << id));

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
    if (inGamePak(sad) && inGamePak(dad))
    {
        cycles_s = 4;
        cycles_n = 4;
    }
    else
    {
        cycles_s = 2;
        cycles_n = 2;
    }

    if (io.control.word)
    {
        cycles_s += arm.io.waitcnt.cyclesWord(sad, true);
        cycles_s += arm.io.waitcnt.cyclesWord(dad, true);
        cycles_n += arm.io.waitcnt.cyclesWord(sad, false);
        cycles_n += arm.io.waitcnt.cyclesWord(dad, false);
    }
    else
    {
        cycles_s += arm.io.waitcnt.cyclesHalf(sad, true);
        cycles_s += arm.io.waitcnt.cyclesHalf(dad, true);
        cycles_n += arm.io.waitcnt.cyclesHalf(sad, false);
        cycles_n += arm.io.waitcnt.cyclesHalf(dad, false);
    }
}

void DMA::updateTransfer()
{
    bool eeprom_w = id == 3 && inEEPROM(dad);
    bool eeprom_r = id == 3 && inEEPROM(sad);

    if ((eeprom_r || eeprom_w) && mmu.gamepak.save->type == Save::Type::Eeprom)
    {
        initEEPROM();

        if (eeprom_w)
        {
            transfer = [&]() {
                u8 byte = static_cast<u8>(mmu.readHalf(sad));
                mmu.gamepak.save->write(dad, byte);
            };
        }
        else
        {
            transfer = [&]() {
                u8 byte = mmu.gamepak.save->read(sad);
                mmu.writeHalf(dad, byte);
            };

        }
    }
    else
    {
        if (io.control.word)
        {
            transfer = [&]() {
                u32 word = mmu.readWord(sad);
                mmu.writeWord(dad, word);
            };
        }
        else
        {
            transfer = [&]() {
                u16 half = mmu.readHalf(sad);
                mmu.writeHalf(dad, half);
            };
        }
    }
}

void DMA::initEEPROM()
{
    // Guessing EEPROM size in advance seems to be pretty much impossible.
    // That's why we base the size on the first write (which should happen
    // before the first read).
    if (mmu.gamepak.save->data.empty())
    {
        switch (remaining)
        {
        // Bus width 6
        case  9:  // Set address for reading
        case 73:  // Write data to address
            mmu.gamepak.save->data.resize(0x0200, 0);
            break;

        // Bus width 14
        case 17:  // Set address for reading
        case 81:  // Write data to address
            mmu.gamepak.save->data.resize(0x2000, 0);
            break;
        }
    }
}
