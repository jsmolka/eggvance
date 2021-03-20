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

void DmaChannel::init()
{
    fifo = (id == 1 || id == 2)
        && control.timing == DmaControl::kTimingSpecial
        && control.repeat
        && dad.isFifo();

    latch.sad   = sad;
    latch.dad   = dad;
    latch.count = fifo ? 4 : static_cast<uint>(count);
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

void DmaChannel::initTransfer()
{
    bool eeprom_r = gamepak.isEepromAccess(latch.sad);
    bool eeprom_w = gamepak.isEepromAccess(latch.dad);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_r)
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000) bus = gamepak.save->read(latch.sad);
                if (latch.dad >= 0x200'0000) arm.writeHalf(latch.dad, bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000) 
                {
                    bus = arm.readHalf(latch.sad, access);
                    bus |= bus << 16;
                }
                if (latch.dad >= 0x200'0000) 
                    gamepak.save->write(latch.dad, bus);
            };
        }
    }
    else
    {
        if (control.word || fifo)
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000) bus = arm.readWord(latch.sad, access);
                if (latch.dad >= 0x200'0000) arm.writeWord(latch.dad, bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000)
                {
                    bus = arm.readHalf(latch.sad, access);
                    bus |= bus << 16;
                }
                if (latch.dad >= 0x200'0000)
                    arm.writeHalf(latch.dad, bus, access);
            };
        }
    }
}

bool DmaChannel::start()
{
    if (fifo)
    {
        if (apu.fifo[latch.dad == 0x400'00A4].size() > 16)
            return false;
    }
    else if (control.repeat)
    {
        latch.count = count;

        if (control.dadcnt == DmaControl::Control::kControlReload)
            latch.dad = dad;
    }

    latch.sad &= ~((2 << control.word) - 1);
    latch.dad &= ~((2 << control.word) - 1);

    running = true;
    pending = latch.count;

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
    int dad_delta = kDeltas[control.word | fifo][fifo ? DmaControl::Control::kControlFixed : control.dadcnt];

    while (pending--)
    {
        if (pending == latch.count - 1)
        {
            if (!(latch.sad & latch.dad & 0x800'0000))
                arm.idle(2);

            transfer(Access::NonSequential);
        }
        else
        {
            transfer(Access::Sequential);
        }

        latch.sad += sad_delta;
        latch.dad += dad_delta;

        if (arm.target >= scheduler.now && pending)
            return;
    }

    running = false;

    if (control.irq)
        arm.raise(kIrqDma0 << id);

    control.setEnabled(control.repeat
        && !(control.timing == DmaControl::Timing::kTimingImmediate)
        && !(control.timing == DmaControl::Timing::kTimingSpecial && id == 3 && ppu.vcount == 161));
}
