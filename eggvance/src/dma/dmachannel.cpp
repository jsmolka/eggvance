#include "dmachannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "gamepak/eeprom.h"
#include "gamepak/gamepak.h"

DmaChannel::DmaChannel(uint id)
    : id(id)
{

}

void DmaChannel::reload()
{
    fifo = control.timing == DmaControl::Timing::kSpecial
        && control.repeat
        && (dad.value == 0x400'00A0 || dad.value == 0x400'00A4)
        && (id == 1 || id == 2);

    internal.count    = fifo ? 4 : count.count(id);
    internal.src_addr = sad.value;
    internal.dst_addr = dad.value;
}

bool DmaChannel::start()
{
    if (fifo && !apu.fifo[internal.dst_addr == 0x400'00A4].needsRefill())
        return false;

    if (control.repeat)
    {
        internal.count = fifo ? 4 : count.count(id);

        if (control.dadcnt == DmaControl::Control::kReload)
            internal.dst_addr = dad.value;
    }

    internal.src_addr &= ~((2 << control.word) - 1);
    internal.dst_addr &= ~((2 << control.word) - 1);

    running = true;
    pending = internal.count;

    initTransfer();

    return true;
}

void DmaChannel::run()
{
    static constexpr int kDeltas[2][4] = {
        { 2, -2, 0, 2 },
        { 4, -4, 0, 4 }
    };

    int delta_src = kDeltas[control.word | fifo][control.sadcnt];
    int delta_dst = kDeltas[control.word | fifo][fifo ? DmaControl::Control::kFixed : control.dadcnt];

    while (pending-- > 0)
    {
        if (pending == internal.count - 1)
        {
            if (!(internal.src_addr & internal.dst_addr & 0x800'0000))
                arm.idle(2);

            transfer(Access::NonSequential);
        }
        else
        {
            transfer(Access::Sequential);
        }

        internal.src_addr += delta_src;
        internal.dst_addr += delta_dst;

        if (arm.cycles <= 0 && pending > 0)
            return;
    }

    running = false;

    if (control.irq)
        arm.raise(kIrqDma0 << id);

    control.enable = control.repeat
        && control.timing != DmaControl::Timing::kImmediate;

    if (!control.enable)
        control.value &= ~(1 << 15);
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
        if (control.word | fifo)
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
