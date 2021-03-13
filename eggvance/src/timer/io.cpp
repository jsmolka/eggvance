#include "io.h"

#include "timerchannel.h"

TimerCount::TimerCount(TimerChannel& channel)
    : channel(channel)
{

}

u8 TimerCount::read(uint index)
{
    SHELL_ASSERT(index < sizeof(u16));

    if (channel.control.cascade == 0)
    {
        channel.run();
        channel.schedule();
    }
    return reinterpret_cast<const u8*>(&counter)[index];
}

void TimerCount::write(uint index, u8 byte)
{
    SHELL_ASSERT(index < sizeof(u16));

    reinterpret_cast<u8*>(&initial)[index] = byte;
}

TimerControl::TimerControl(TimerChannel& channel)
    : channel(channel)
{

}

void TimerControl::write(uint index, u8 byte)
{
    if (index == 1 || bytes[index] == byte)
        return;

    XRegister::write(index, byte);

    constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

    if (channel.control.cascade == 0)
        channel.run();

    uint was_enabled = enabled;

    prescaler = kPrescalers[bit::seq<0, 3>(byte)];
    cascade   = bit::seq<2, 1>(byte);
    irq       = bit::seq<6, 1>(byte);
    enabled   = bit::seq<7, 1>(byte);

    if (channel.id == 0)
        cascade = 0;

    if (!was_enabled && enabled)
        channel.start();
    else
        channel.update();
}
