#include "dmachannel.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "gamepak/eeprom.h"
#include "gamepak/gamepak.h"
#include "mmu/mmu.h"

DmaChannel::DmaChannel(uint id)
    : id(id)
{

}

void DmaChannel::reload()
{
    cache.count    = count.count(id);
    cache.src_addr = sad.value;
    cache.dst_addr = dad.value;
}

void DmaChannel::start()
{
    enum Control
    {
        kControlIncrement,
        kControlDecrement,
        kControlFixed,
        kControlReload
    };

    if (control.repeat)
    {
        cache.count = count.count(id);

        if (control.dadcnt == kControlReload)
            cache.dst_addr = dad.value;
    }

    running = true;
    pending = cache.count;

    cache.src_addr &= ~((2 << control.word) - 1);
    cache.dst_addr &= ~((2 << control.word) - 1);

    initCycles();
    initTransfer();
}

void DmaChannel::run(int& cycles)
{
    while (pending-- > 0)
    {
        static constexpr int kDeltas[2][4] = {
            { 2, -2, 0, 2 },
            { 4, -4, 0, 4 }
        };

        transfer();

        cache.src_addr += kDeltas[control.word][control.sadcnt];
        cache.dst_addr += kDeltas[control.word][control.dadcnt];

        cycles -= pending == 0
            ? cycles_n
            : cycles_s;

        if (cycles <= 0 && pending > 0)
            return;
    }

    running = false;

    if (control.irq)
        arm.raise(kIrqDma0 << id);

    control.enable = control.repeat
        && control.timing != DmaControl::kTimingImmediate;

    if (!control.enable)
        control.value &= ~DmaControl::kEnable;
}

bool DmaChannel::isGamePak(u32 addr)
{
    return addr >= 0x800'0000 && addr < 0xE00'0000;
}

void DmaChannel::initCycles()
{
    if (isGamePak(cache.src_addr) && isGamePak(cache.dst_addr))
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
        cycles_n += arm.waitcnt.cyclesWord(cache.src_addr, Access::NonSequential);
        cycles_n += arm.waitcnt.cyclesWord(cache.dst_addr, Access::NonSequential);
        cycles_s += arm.waitcnt.cyclesWord(cache.src_addr, Access::Sequential);
        cycles_s += arm.waitcnt.cyclesWord(cache.dst_addr, Access::Sequential);
    }
    else
    {
        cycles_n += arm.waitcnt.cyclesHalf(cache.src_addr, Access::NonSequential);
        cycles_n += arm.waitcnt.cyclesHalf(cache.dst_addr, Access::NonSequential);
        cycles_s += arm.waitcnt.cyclesHalf(cache.src_addr, Access::Sequential);
        cycles_s += arm.waitcnt.cyclesHalf(cache.dst_addr, Access::Sequential);
    }
}

void DmaChannel::initTransfer()
{
    bool eeprom_w = gamepak.isEepromAccess(cache.dst_addr);
    bool eeprom_r = gamepak.isEepromAccess(cache.src_addr);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_w)
        {
            transfer = [&]() {
                if (cache.src_addr >= 0x200'0000) bus = mmu.readHalf(cache.src_addr);
                if (cache.dst_addr >= 0x200'0000) gamepak.save->write(cache.dst_addr, bus);
            };
        }
        else
        {
            transfer = [&]() {
                if (cache.src_addr >= 0x200'0000) bus = gamepak.save->read(cache.src_addr);
                if (cache.dst_addr >= 0x200'0000) mmu.writeHalf(cache.dst_addr, bus);
            };
        }
    }
    else
    {
        if (control.word)
        {
            transfer = [&]() {
                if (cache.src_addr >= 0x200'0000) bus = mmu.readWord(cache.src_addr);
                if (cache.dst_addr >= 0x200'0000) mmu.writeWord(cache.dst_addr, bus);
            };
        }
        else
        {
            transfer = [&]() {
                if (cache.src_addr >= 0x200'0000) bus = mmu.readHalf(cache.src_addr);
                if (cache.dst_addr >= 0x200'0000) mmu.writeHalf(cache.dst_addr, bus);
            };
        }
    }
}

void DmaChannel::initEeprom()
{
    if (!gamepak.save->data.empty())
        return;

    constexpr uint kBus6Write = 73;
    constexpr uint kBus6ReadSetAddress = 9;
    constexpr uint kBus14Write = 81;
    constexpr uint kBus14ReadSetAddress = 17;

    switch (pending)
    {
    case kBus6Write:
    case kBus6ReadSetAddress:
        gamepak.save->data.resize(Eeprom::kSize4, 0xFF);
        break;

    case kBus14Write:
    case kBus14ReadSetAddress:
        gamepak.save->data.resize(Eeprom::kSize64, 0xFF);
        break;
    }
}
