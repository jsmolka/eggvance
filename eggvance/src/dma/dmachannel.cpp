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
    internal.count    = count.count(id);
    internal.src_addr = sad.value;
    internal.dst_addr = dad.value;
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
        internal.count = count.count(id);

        if (control.dadcnt == kControlReload)
            internal.dst_addr = dad.value;
    }

    running = true;
    pending = internal.count;

    internal.src_addr &= ~((2 << control.word) - 1);
    internal.dst_addr &= ~((2 << control.word) - 1);

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

        internal.src_addr += kDeltas[control.word][control.sadcnt];
        internal.dst_addr += kDeltas[control.word][control.dadcnt];

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
    if (isGamePak(internal.src_addr) && isGamePak(internal.dst_addr))
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
        cycles_n += arm.waitcnt.cyclesWord(internal.src_addr, Access::NonSequential);
        cycles_n += arm.waitcnt.cyclesWord(internal.dst_addr, Access::NonSequential);
        cycles_s += arm.waitcnt.cyclesWord(internal.src_addr, Access::Sequential);
        cycles_s += arm.waitcnt.cyclesWord(internal.dst_addr, Access::Sequential);
    }
    else
    {
        cycles_n += arm.waitcnt.cyclesHalf(internal.src_addr, Access::NonSequential);
        cycles_n += arm.waitcnt.cyclesHalf(internal.dst_addr, Access::NonSequential);
        cycles_s += arm.waitcnt.cyclesHalf(internal.src_addr, Access::Sequential);
        cycles_s += arm.waitcnt.cyclesHalf(internal.dst_addr, Access::Sequential);
    }
}

void DmaChannel::initTransfer()
{
    bool eeprom_r = gamepak.isEepromAccess(internal.src_addr);
    bool eeprom_w = gamepak.isEepromAccess(internal.dst_addr);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_r)
        {
            transfer = [&]() {
                if (internal.src_addr >= 0x200'0000) bus = gamepak.save->read(internal.src_addr);
                if (internal.dst_addr >= 0x200'0000) mmu.writeHalf(internal.dst_addr, bus);
            };
        }
        else
        {
            transfer = [&]() {
                if (internal.src_addr >= 0x200'0000) 
                {
                    bus = mmu.readHalf(internal.src_addr);
                    bus |= bus << 16;
                }
                if (internal.dst_addr >= 0x200'0000) 
                    gamepak.save->write(internal.dst_addr, bus);
            };
        }
    }
    else
    {
        if (control.word)
        {
            transfer = [&]() {
                if (internal.src_addr >= 0x200'0000) bus = mmu.readWord(internal.src_addr);
                if (internal.dst_addr >= 0x200'0000) mmu.writeWord(internal.dst_addr, bus);
            };
        }
        else
        {
            transfer = [&]() {
                if (internal.src_addr >= 0x200'0000)
                {
                    bus = mmu.readHalf(internal.src_addr);
                    bus |= bus << 16;
                }
                if (internal.dst_addr >= 0x200'0000)
                    mmu.writeHalf(internal.dst_addr, bus);
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
