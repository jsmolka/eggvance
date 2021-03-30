#include "io.h"

#include "timerchannel.h"

TimerCount::TimerCount(TimerChannel& channel)
    : channel(channel)
{

}

u8 TimerCount::read(uint index)
{
    if (channel.control.runnable())
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
    : Register(channel.id == 0 ? 0x00C3 : 0x00C7), channel(channel)
{

}

void TimerControl::write(uint index, u8 byte)
{
    static constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

    if (index == 1 || read(index) == byte)
        return;

    Register::write(index, byte);

    if (runnable())
        channel.run();

    uint was_enabled = enabled;

    prescaler = kPrescalers[bit::seq<0, 3>(data)];
    cascade   = bit::seq<2, 1>(data);
    irq       = bit::seq<6, 1>(data);
    enabled   = bit::seq<7, 1>(data);

    if (!was_enabled && enabled)
        channel.start();
    else
        channel.update();
}

bool TimerControl::runnable() const
{
    return enabled && !cascade;
}
