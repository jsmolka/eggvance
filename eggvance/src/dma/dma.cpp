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

Dma::Dma(uint id)
{
    this->id = id;
}

void Dma::start()
{
    running   = true;
    remaining = io.count.count(id);

    if (io.control.reload)
    {
        sad = io.sad.value;
        dad = io.dad.value;
        io.control.reload = false;
    }
    else if (io.control.repeat && io.control.dadcnt == kAddressControlReload)
    {
        dad = io.dad.value;
    }

    uint size = 2 << io.control.word;

    sad &= ~(size - 1);
    dad &= ~(size - 1);

    sad_delta = size * deltas[io.control.sadcnt];
    dad_delta = size * deltas[io.control.dadcnt];

    initCycles();
    initTransfer();
}

void Dma::run(int& cycles)
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
        irqh.request(kIrqDma0 << id);

    running = false;
}

bool Dma::isEeprom(u32 addr)
{
    return mmu.gamepak.size() <= 0x100'0000
        ? addr >= 0xD00'0000 && addr < 0xE00'0000
        : addr >= 0xDFF'FF00 && addr < 0xE00'0000;
}

bool Dma::isGamePak(u32 addr)
{
    return addr >= 0x800'0000 && addr < 0xE00'0000;
}

void Dma::initCycles()
{
    if (isGamePak(sad) && isGamePak(dad))
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

void Dma::initTransfer()
{
    bool eeprom_w = id == 3 && isEeprom(dad);
    bool eeprom_r = id == 3 && isEeprom(sad);

    if ((eeprom_r || eeprom_w) && mmu.gamepak.save->type == Save::Type::Eeprom)
    {
        initEeprom();

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

void Dma::initEeprom()
{
    constexpr uint kBus6Write = 73;
    constexpr uint kBus6ReadSetAddress = 9;
    constexpr uint kBus14Write = 81;
    constexpr uint kBus14ReadSetAddress = 17;

    if (mmu.gamepak.save->data.empty())
    {
        switch (remaining)
        {
        case kBus6Write:
        case kBus6ReadSetAddress:
            mmu.gamepak.save->data.resize(0x0200, 0xFF);
            break;

        case kBus14Write:
        case kBus14ReadSetAddress:
            mmu.gamepak.save->data.resize(0x2000, 0xFF);
            break;
        }
    }
}
