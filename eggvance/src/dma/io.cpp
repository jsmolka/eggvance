#include "io.h"

#include <shell/operators.h>

#include "dma.h"

DmaSrcAddress::DmaSrcAddress(uint id)
    : RegisterW(id == 0 ? 0x07FF'FFFF : 0x0FFF'FFFF)
{

}

DmaSrcAddress::operator u32() const
{
    return data;
}

DmaDstAddress::DmaDstAddress(uint id)
    : RegisterW(id == 3 ? 0x0FFF'FFFF : 0x07FF'FFFF)
{

}

DmaDstAddress::operator u32() const
{
    return data;
}

DmaCount::DmaCount(uint id)
    : RegisterW(id == 3 ? 0xFFFF : 0x3FFF)
{

}

DmaCount::operator uint() const
{
    if (data == 0)
        return mask + 1;
    else
        return data;
}

DmaControl::DmaControl(DmaChannel& channel)
    : Register(channel.id == 3 ? 0xFFE0 : 0xF7E0), channel(channel)
{

}

void DmaControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    dadcnt = bit::seq<5, 2>(data);
    sadcnt = bit::seq<7, 2>(data);

    if (index == 0)
        return;

    uint was_enabled = enabled;

    repeat  = bit::seq<1, 1>(byte);
    word    = bit::seq<2, 1>(byte);
    timing  = bit::seq<4, 2>(byte);
    irq     = bit::seq<6, 1>(byte);
    enabled = bit::seq<7, 1>(byte);

    if (!was_enabled && enabled)
    {
        channel.init();

        if (timing == Timing::Immediate)
            dma.emit(channel, Dma::Event::Immediate);
    }
}

void DmaControl::setEnabled(bool enabled)
{
    constexpr auto kEnabled = 1 << 15;

    if (enabled)
        data |=  kEnabled;
    else
        data &= ~kEnabled;

    this->enabled = enabled;
}
