#include "io.h"

#include "apu.h"
#include "base/config.h"

SoundControl::SoundControl()
{
    if (config.bios_skip)
    {
        write(2, 0x0E);
    }
}

u8 SoundControl::read(uint index) const
{
    u8 value = XRegister::read(index);

    if (index == 4)
    {
        value |= apu.square1.enabled << 0;
        value |= apu.square2.enabled << 1;
        value |= apu.wave.enabled    << 2;
        value |= apu.noise.enabled   << 3;
    }
    return value;
}

void SoundControl::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    switch (index)
    {
    case 0:
        volume_r = bit::seq<0, 3>(byte);
        volume_l = bit::seq<4, 3>(byte);
        break;

    case 1:
        enabled_r = bit::seq<0, 4>(byte);
        enabled_l = bit::seq<4, 4>(byte);
        break;

    case 2:
        volume             = bit::seq<0, 2>(byte);
        apu.fifo[0].volume = bit::seq<2, 1>(byte);
        apu.fifo[1].volume = bit::seq<3, 1>(byte);
        break;

    case 3:
        apu.fifo[0].enabled_r = bit::seq<0, 1>(byte);
        apu.fifo[0].enabled_l = bit::seq<1, 1>(byte);
        apu.fifo[0].timer     = bit::seq<2, 1>(byte);
        apu.fifo[1].enabled_r = bit::seq<4, 1>(byte);
        apu.fifo[1].enabled_l = bit::seq<5, 1>(byte);
        apu.fifo[1].timer     = bit::seq<6, 1>(byte);
        
        if (byte & (1 << 3)) apu.fifo[0].clear();
        if (byte & (1 << 7)) apu.fifo[1].clear();
        break;

    case 4:
        enabled = bit::seq<7, 1>(byte);
        break;
    }
}

SoundBias::SoundBias()
{
    if (config.bios_skip)
    {
        write(1, 0x02);
    }
}

SoundBias::operator uint() const
{
    return level;
}

void SoundBias::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    level = bit::seq<0, 10>(value);
}
