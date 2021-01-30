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

        timer = period();
    }
}
