#include "io.h"

DmaSource::DmaSource(uint id)
    : XRegisterW(id == 0 ? 0x07FF'FFFF : 0x0FFF'FFFF)
{

}

DmaSource::operator u32() const
{
    return value;
}

void DmaSource::write(uint index, u8 byte)
{
    XRegisterW::write(index, byte);

    value &= mask;
}

DmaDestination::DmaDestination(uint id)
    : XRegisterW(id == 3 ? 0x0FFF'FFFF : 0x07FF'FFFF)
{

}

DmaDestination::operator u32() const
{
    return value;
}

void DmaDestination::write(uint index, u8 byte)
{
    XRegisterW::write(index, byte);

    value &= mask;
}

DmaCount::DmaCount(uint id)
    : XRegisterW(id == 3 ? 0xFFFF : 0x3FFF)
{

}

DmaCount::operator uint() const
{
    if (value == 0)
        return mask + 1;
    else
        return value;
}

void DmaCount::write(uint index, u8 byte)
{
    XRegisterW::write(index, byte);

    value &= mask;
}

DmaControl::DmaControl(uint id)
    : XRegister(id == 3 ? 0xFFE0 : 0xF7E0)
{

}

void DmaControl::write(uint index, u8 byte)
{
    // Todo: return here?, use something better than value?
    XRegister::write(index, byte);

    value &= mask;

    sadcnt = bit::seq<7, 2>(value);

    if (index == 0)
    {
        dadcnt = bit::seq<5, 2>(byte);
    }
    else
    {
        uint was_enable = enable;

        repeat = bit::seq< 9, 1>(value);
        word   = bit::seq<10, 1>(value);
        drq    = bit::seq<11, 1>(value);
        timing = bit::seq<12, 2>(value);
        irq    = bit::seq<14, 1>(value);
        enable = bit::seq<15, 1>(value);

        on_write(!was_enable && enable);
    }
}
