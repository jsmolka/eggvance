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

void DmaChannel::start()
{
    enum AddressControl
    {
        kAddressControlIncrement,
        kAddressControlDecrement,
        kAddressControlFixed,
        kAddressControlReload
    };

    running = true;
    pending = count.count(id);

    if (control.reload)
    {
        control.reload = false;

        src_addr = sad.value;
        dst_addr = dad.value;
    }
    else if (control.repeat && control.dadcnt == kAddressControlReload)
    {
        dst_addr = dad.value;
    }

    src_addr &= ~((2 << control.word) - 1);
    dst_addr &= ~((2 << control.word) - 1);

    initCycles();
    initTransfer();
}

void DmaChannel::run(int& cycles)
{
    static constexpr int kDeltas[2][4] = {
        { 2, -2, 0, 2 },
        { 4, -4, 0, 4 }
    };

    while (pending-- > 0)
    {
        transfer();

        src_addr += kDeltas[control.word][control.sadcnt];
        dst_addr += kDeltas[control.word][control.dadcnt];

        if (pending == 0)
            cycles -= cycles_n;
        else
            cycles -= cycles_s;

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
    if (isGamePak(src_addr) && isGamePak(dst_addr))
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
        cycles_s += arm.waitcnt.cyclesWord(src_addr, true);
        cycles_s += arm.waitcnt.cyclesWord(dst_addr, true);
        cycles_n += arm.waitcnt.cyclesWord(src_addr, false);
        cycles_n += arm.waitcnt.cyclesWord(dst_addr, false);
    }
    else
    {
        cycles_s += arm.waitcnt.cyclesHalf(src_addr, true);
        cycles_s += arm.waitcnt.cyclesHalf(dst_addr, true);
        cycles_n += arm.waitcnt.cyclesHalf(src_addr, false);
        cycles_n += arm.waitcnt.cyclesHalf(dst_addr, false);
    }
}

void DmaChannel::initTransfer()
{
    bool eeprom_w = gamepak.isEeprom(dst_addr);
    bool eeprom_r = gamepak.isEeprom(src_addr);

    if ((eeprom_r || eeprom_w) && id == 3)
    {
        initEeprom();

        if (eeprom_w)
        {
            transfer = [&]() {
                u8 byte = mmu.readHalf(src_addr);
                gamepak.save->write(dst_addr, byte);
            };
        }
        else
        {
            transfer = [&]() {
                u8 byte = gamepak.save->read(src_addr);
                mmu.writeHalf(dst_addr, byte);
            };
        }
    }
    else
    {
        if (control.word)
        {
            transfer = [&]() {
                u32 word = mmu.readWord(src_addr);
                mmu.writeWord(dst_addr, word);
            };
        }
        else
        {
            transfer = [&]() {
                u16 half = mmu.readHalf(src_addr);
                mmu.writeHalf(dst_addr, half);
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

    if (gamepak.save->data.empty())
    {
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
}
