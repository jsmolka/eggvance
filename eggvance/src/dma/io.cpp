#include "io.h"

DmaSource::DmaSource(uint id)
    : RegisterW(id == 0 ? 0x07FF'FFFF : 0x0FFF'FFFF)
{

}

DmaSource::operator u32() const
{
    return data;
}

DmaDestination::DmaDestination(uint id)
    : RegisterW(id == 3 ? 0x0FFF'FFFF : 0x07FF'FFFF)
{

}

DmaDestination::operator u32() const
{
    return data;
}

bool DmaDestination::isFifo() const
{
    return data == 0x400'00A0 || data == 0x400'00A4;
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

DmaControl::DmaControl(uint id)
    : Register(id == 3 ? 0xFFE0 : 0xF7E0)
{

}

void DmaControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    sadcnt = bit::seq<7, 2>(data);

    if (index == 0)
    {
        dadcnt = bit::seq<5, 2>(byte);
    }
    else
    {
        uint was_enable = enabled;

        repeat = bit::seq<1, 1>(byte);
        word   = bit::seq<2, 1>(byte);
        timing = bit::seq<4, 2>(byte);
        irq    = bit::seq<6, 1>(byte);
        enabled = bit::seq<7, 1>(byte);

        on_write(!was_enable && enabled);
    }
}

void DmaControl::setEnabled(bool enabled)
{
    constexpr auto kEnabled = 1 << 15;

    this->enabled = enabled;

    if (enabled)
        data |=  kEnabled;
    else
        data &= ~kEnabled;
}
