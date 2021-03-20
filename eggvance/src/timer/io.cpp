#include "io.h"

#include "timerchannel.h"

TimerCount::TimerCount(TimerChannel& channel)
    : channel(channel)
{

}

u8 TimerCount::read(uint index)
{
    if (channel.control.enabled && !channel.control.cascade)
    {
        channel.run();
        channel.schedule();
    }
    return bit::byte(counter, index);
}

void TimerCount::write(uint index, u8 byte)
{
    bit::byteRef(initial, index) = byte;
}

TimerControl::TimerControl(TimerChannel& channel)
    : Register(channel.id == 0 ? 0x00C3 : 0x00C7)
    , channel(channel)
{

}

void TimerControl::write(uint index, u8 byte)
{
    if (index == 1 || !Register::write(index, byte))
        return;

    constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

    value &= mask;

    if (enabled && !cascade)
        channel.run();

    uint was_enabled = enabled;

    prescaler = kPrescalers[bit::seq<0, 3>(value)];
    cascade   = bit::seq<2, 1>(value);
    irq       = bit::seq<6, 1>(value);
    enabled   = bit::seq<7, 1>(value);

    if (!was_enabled && enabled)
        channel.start();
    else
        channel.update();
}
