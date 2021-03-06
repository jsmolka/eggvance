#include "timer.h"

Timer::Timer()
{
    channels[1].pred = &channels[0];
    channels[2].pred = &channels[1];
    channels[3].pred = &channels[2];

    channels[0].succ = &channels[1];
    channels[1].succ = &channels[2];
    channels[2].succ = &channels[3];
}
