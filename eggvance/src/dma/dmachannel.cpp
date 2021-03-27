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

    latch.sad   = sad & ~((2 << control.word) - 1);
    latch.dad   = dad & ~((2 << control.word) - 1);
    latch.count = fifo ? 4 : static_cast<uint>(count);

    if (sad.isGamePak())
        control.sadcnt = DmaControl::kControlIncrement;

    initTransfer();
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

        if (control.dadcnt == DmaControl::kControlReload)
            latch.dad = dad & ~((2 << control.word) - 1);
    }

    running = true;
    pending = latch.count;

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
    int dad_delta = kDeltas[control.word | fifo][fifo ? DmaControl::kControlFixed : control.dadcnt];

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
        arm.raise(Irq::Dma << id);

    control.setEnabled(control.repeat
        && !(control.timing == DmaControl::kTimingImmediate)
        && !(control.timing == DmaControl::kTimingSpecial && id == 3 && ppu.vcount >= 161));
}

void DmaChannel::initEeprom()
{
    if (!gamepak.save->data.empty())
        return;

    constexpr auto kBus6Write = 73;
    constexpr auto kBus6ReadSetAddress = 9;
    constexpr auto kBus14Write = 81;
    constexpr auto kBus14ReadSetAddress = 17;

    switch (latch.count)
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
                if (latch.sad >= 0x200'0000)
                    latch.bus = gamepak.save->read(latch.sad);
                
                arm.writeHalf(latch.dad, latch.bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000) 
                    latch.bus = arm.readHalf(latch.sad, access) * 0x0001'0001;

                gamepak.save->write(latch.dad, latch.bus);
            };
        }
    }
    else
    {
        if (control.word || fifo)
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000)
                    latch.bus = arm.readWord(latch.sad, access);
                
                arm.writeWord(latch.dad, latch.bus, access);
            };
        }
        else
        {
            transfer = [this](Access access)
            {
                if (latch.sad >= 0x200'0000)
                    latch.bus = arm.readHalf(latch.sad, access) * 0x0001'0001;

                arm.writeHalf(latch.dad, latch.bus, access);
            };
        }
    }
}
