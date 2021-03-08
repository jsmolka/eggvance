#include "io.h"

#include <shell/log/all.h>

#include "timerchannel.h"

TimerRegister::TimerRegister(TimerChannel& channel)
    : channel(channel)
{

}

void TimerRegister::run()
{
    TimerChannel* timer = &channel;

    if (!timer->control.enabled)
        return;

    while (timer->control.cascade)
    {
        timer = timer->pred;

        if (!timer || !timer->control.enabled)
            return;
    }

    timer->run();
    timer->schedule();
}

u8 TimerCount::read(uint index)
{
    run();

    return XRegister::read(index);
}

void TimerCount::write(uint index, u8 byte)
{
    SHELL_ASSERT(index < sizeof(initial));

    reinterpret_cast<u8*>(&initial)[index] = byte;
}

void TimerControl::write(uint index, u8 byte)
{
    if (index == 0 && bytes[index] == byte)
    {
        //SHELL_LOG_INFO("useless control write");
        return;
    }

    XRegister::write(index, byte);

    if (index == 0)
    {
        static constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

        run();

        uint was_enabled = enabled;

        prescaler = kPrescalers[bit::seq<0, 3>(byte)];
        cascade   = bit::seq<2, 1>(byte);
        irq       = bit::seq<6, 1>(byte);
        enabled   = bit::seq<7, 1>(byte);

        if (!was_enabled && enabled)
            channel.start();
        else
            channel.update();
    }
}
