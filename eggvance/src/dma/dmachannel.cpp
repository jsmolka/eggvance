#include "dmachannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "gamepak/eeprom.h"
#include "gamepak/gamepak.h"
#include "ppu/ppu.h"

DmaChannel::DmaChannel(uint id)
    : id(id)
    , sad(id)
    , dad(id)
    , count(id)
    , control(id)
{

}

void DmaChannel::reload()
{
    fifo = (id == 1 || id == 2)
        && control.timing == DmaControl::Timing::kSpecial
        && control.repeat
        && dad.isFifo();

    internal.sad   = sad;
    internal.dad   = dad;
    internal.count = fifo ? 4 : static_cast<uint>(count);
}

bool DmaChannel::start()
{
    if (fifo && !apu.fifo[internal.dad == 0x400'00A4].refillable())
        return false;

    if (control.repeat)
    {
        internal.count = fifo ? 4 : static_cast<int>(count);

        if (control.dadcnt == DmaControl::Control::kReload)
            internal.dad = dad;
    }

    uint mask = ~((2 << control.word) - 1);

    internal.sad &= mask;
    internal.dad &= mask;

    running = true;
    pending = internal.count;

    initTransfer();

    return true;
}

void DmaChannel::run()
{
    static constexpr int kDeltas[2][4] =
    {
        { 2, -2, 0, 2 },
        { 4, -4, 0, 4 }
    };

    int sad_delta = kDeltas[control.word | fifo][control.sadcnt];
    int dad_delta = kDeltas[control.word | fifo][fifo ? DmaControl::Control::kFixed : control.dadcnt];

    while (pending--)
    {
        if (pending == internal.count - 1)
        {
            if (!(internal.sad & internal.dad & 0x800'0000))
                arm.idle(2);

            transfer(Access::NonSequential);
        }
        else
        {
            transfer(Access::Sequential);
        }

        internal.sad += sad_delta;
        internal.dad += dad_delta;

        if (arm.target >= scheduler.now && pending)
            return;
    }

    running = false;

    if (control.irq)
        arm.raise(kIrqDma0 << id);

    control.setEnabled(control.repeat
        && !(control.timing == DmaControl::Timing::kImmediate)
        && !(control.timing == DmaControl::Timing::kSpecial && id == 3 && ppu.vcount == 161));
}

void DmaChannel::initTransfer()
{
    bool eeprom_r = gamepak.isEepromAccess(internal.sad);
    bool eeprom_w = gamepak.isEepromAccess(internal.dad);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_r)
        {
            transfer = [this](Access access)
            {
                if (internal.sad >= 0x200'0000) bus = gamepak.save->read(internal.sad);
                if (internal.dad >= 0x200'0000) arm.writeHalf(internal.dad, bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (internal.sad >= 0x200'0000) 
                {
                    bus = arm.readHalf(internal.sad, access);
                    bus |= bus << 16;
                }
                if (internal.dad >= 0x200'0000) 
                    gamepak.save->write(internal.dad, bus);
            };
        }
    }
    else
    {
        if (control.word | fifo)
        {
            transfer = [this](Access access)
            {
                if (internal.sad >= 0x200'0000) bus = arm.readWord(internal.sad, access);
                if (internal.dad >= 0x200'0000) arm.writeWord(internal.dad, bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (internal.sad >= 0x200'0000)
                {
                    bus = arm.readHalf(internal.sad, access);
                    bus |= bus << 16;
                }
                if (internal.dad >= 0x200'0000)
                    arm.writeHalf(internal.dad, bus, access);
            };
        }
    }
}

void DmaChannel::initEeprom()
{
    if (!gamepak.save->data.empty())
        return;

    constexpr auto kBus6Write = 73;
    constexpr auto kBus6ReadSetAddress = 9;
    constexpr auto kBus14Write = 81;
    constexpr auto kBus14ReadSetAddress = 17;

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
