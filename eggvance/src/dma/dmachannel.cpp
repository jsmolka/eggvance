#include "dmachannel.h"

#include "arm/arm.h"
#include "irq/irqh.h"
#include "mmu/mmu.h"

enum AddressControl
{
    kAddressControlIncrement,
    kAddressControlDecrement,
    kAddressControlFixed,
    kAddressControlReload
};

void DmaChannel::start()
{
    running = true;
    pending = count.count(id);

    if (control.reload)
    {
        control.reload = false;

        sad_value = sad.value;
        dad_value = dad.value;
    }
    else if (control.repeat && control.dadcnt == kAddressControlReload)
    {
        dad_value = dad.value;
    }

    sad_value &= ~((2 << control.word) - 1);
    dad_value &= ~((2 << control.word) - 1);

    initCycles();
    initTransfer();
}

void DmaChannel::run(int& cycles)
{
    static constexpr int kDeltas[2][4] =
    {
        { 2, -2, 0, 2 },
        { 4, -4, 0, 4 }
    };

    while (pending-- > 0)
    {
        transfer();

        sad_value += kDeltas[control.word][control.sadcnt];
        dad_value += kDeltas[control.word][control.dadcnt];

        if (pending == 0)
            cycles -= cycles_n;
        else
            cycles -= cycles_s;

        if (cycles <= 0)
            return;
    }

    running = false;

    if (!(control.enable = control.repeat))
        control.value &= ~DmaControl::kEnable;

    if (control.irq)
        irqh.request(kIrqDma0 << id);
}

bool DmaChannel::isEeprom(u32 addr)
{
    return mmu.gamepak.size() <= 0x100'0000
        ? addr >= 0xD00'0000 && addr < 0xE00'0000
        : addr >= 0xDFF'FF00 && addr < 0xE00'0000;
}

bool DmaChannel::isGamePak(u32 addr)
{
    return addr >= 0x800'0000 && addr < 0xE00'0000;
}

void DmaChannel::initCycles()
{
    if (isGamePak(sad_value) && isGamePak(dad_value))
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
        cycles_s += arm.io.waitcnt.cyclesWord(sad_value, true);
        cycles_s += arm.io.waitcnt.cyclesWord(dad_value, true);
        cycles_n += arm.io.waitcnt.cyclesWord(sad_value, false);
        cycles_n += arm.io.waitcnt.cyclesWord(dad_value, false);
    }
    else
    {
        cycles_s += arm.io.waitcnt.cyclesHalf(sad_value, true);
        cycles_s += arm.io.waitcnt.cyclesHalf(dad_value, true);
        cycles_n += arm.io.waitcnt.cyclesHalf(sad_value, false);
        cycles_n += arm.io.waitcnt.cyclesHalf(dad_value, false);
    }
}

void DmaChannel::initTransfer()
{
    bool eeprom_w = id == 3 && isEeprom(dad_value);
    bool eeprom_r = id == 3 && isEeprom(sad_value);

    if ((eeprom_r || eeprom_w) && mmu.gamepak.save->type == Save::Type::Eeprom)
    {
        initEeprom();

        if (eeprom_w)
        {
            transfer = [&]() {
                u8 byte = static_cast<u8>(mmu.readHalf(sad_value));
                mmu.gamepak.save->write(dad_value, byte);
            };
        }
        else
        {
            transfer = [&]() {
                u8 byte = mmu.gamepak.save->read(sad_value);
                mmu.writeHalf(dad_value, byte);
            };
        }
    }
    else
    {
        if (control.word)
        {
            transfer = [&]() {
                u32 word = mmu.readWord(sad_value);
                mmu.writeWord(dad_value, word);
            };
        }
        else
        {
            transfer = [&]() {
                u16 half = mmu.readHalf(sad_value);
                mmu.writeHalf(dad_value, half);
            };
        }
    }
}

void DmaChannel::initEeprom()
{
    constexpr uint kBus6Write = 73;
    constexpr uint kBus6ReadSetAddress = 9;
    constexpr uint kBus14Write = 81;
    constexpr uint kBus14ReadSetAddress = 17;

    if (mmu.gamepak.save->data.empty())
    {
        switch (pending)
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
