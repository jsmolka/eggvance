#include "channel.h"

Channel::Channel(u64 mask, uint base)
    : XRegister(mask), length(base)
{

}

void Channel::tickSweep()
{
    if (enabled && sweep.tick())
    {
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
        enabled = length.enabled();
    }
}

void Channel::tickEnvelope()
{
    if (enabled)
    {
        envelope.tick();
        enabled = envelope.enabled();
    }
}

void Channel::init(bool enabled)
{
    this->enabled = enabled;

    length.init();

    timer = period();
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
