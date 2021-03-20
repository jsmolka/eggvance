#include "channel.h"

#include "scheduler/scheduler.h"

Channel::Channel(u64 mask, uint length)
    : Register(mask), length(length)
{

}

void Channel::tickSweep()
{
    if (enabled && sweep.tick())
    {
        tick();

        doSweep(true);
        doSweep(false);

        sweep.init();
    }
}

void Channel::tickLength()
{
    if (enabled)
    {
        length.tick();

        if (!(enabled = length.enabled()))
            tick();
    }
}

void Channel::tickEnvelope()
{
    if (enabled)
    {
        envelope.tick();

        if (!(enabled = envelope.enabled()))
            tick();
    }
}

void Channel::init(bool enabled)
{
    this->enabled = enabled;

    length.init();

    timer = 0;
    since = scheduler.now;
}

void Channel::initSweep()
{
    sweep.init(frequency);
    if (sweep.shift)
        doSweep(false);
}

void Channel::initEnvelope()
{
    envelope.init();
    enabled &= envelope.enabled();
}

uint Channel::run()
{
    timer += scheduler.now - since;

    uint period = this->period();
    uint ticks  = timer / period;

    timer %= period;
    since = scheduler.now;

    return ticks;
}

void Channel::write(uint index, u8 byte)
{
    if (enabled)
        tick();

    Register::write(index, byte);
}

void Channel::doSweep(bool writeback)
{
    uint freq = sweep.next();
    if  (freq > 2047)
    {
        enabled = false;
    }
    else if (writeback && sweep.shift)
    {
        frequency    = freq;
        sweep.shadow = freq;
    }
}
