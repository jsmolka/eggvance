#include "dmachannel.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "gamepak/eeprom.h"
#include "gamepak/gamepak.h"

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

    initTransfer();
}

void DmaChannel::run()
{
    while (pending-- > 0)
    {
        if (pending == 0)
        {
            if (!((internal.src_addr & 0x800'0000) && (internal.dst_addr & 0x800'0000)))
            {
                arm.idle();
                arm.idle();
            }
            transfer(Access::NonSequential);
        }
        else
        {
            transfer(Access::Sequential);
        }

        static constexpr int kDeltas[2][4] = {
            { 2, -2, 0, 2 },
            { 4, -4, 0, 4 }
        };

        internal.src_addr += kDeltas[control.word][control.sadcnt];
        internal.dst_addr += kDeltas[control.word][control.dadcnt];

        if (arm.cycles <= 0 && pending > 0)
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

void DmaChannel::initTransfer()
{
    bool eeprom_r = gamepak.isEepromAccess(internal.src_addr);
    bool eeprom_w = gamepak.isEepromAccess(internal.dst_addr);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_r)
        {
            transfer = [&](Access access) {
                if (internal.src_addr >= 0x200'0000) bus = gamepak.save->read(internal.src_addr);
                if (internal.dst_addr >= 0x200'0000) arm.writeHalf(internal.dst_addr, bus, access);
            };
        }
        else
        {
            transfer = [&](Access access) {
                if (internal.src_addr >= 0x200'0000) 
                {
                    bus = arm.readHalf(internal.src_addr, access);
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
            transfer = [&](Access access) {
                if (internal.src_addr >= 0x200'0000) bus = arm.readWord(internal.src_addr, access);
                if (internal.dst_addr >= 0x200'0000) arm.writeWord(internal.dst_addr, bus, access);
            };
        }
        else
        {
            transfer = [&](Access access) {
                if (internal.src_addr >= 0x200'0000)
                {
                    bus = arm.readHalf(internal.src_addr, access);
                    bus |= bus << 16;
                }
                if (internal.dst_addr >= 0x200'0000)
                    arm.writeHalf(internal.dst_addr, bus, access);
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
