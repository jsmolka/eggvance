#include "timer.h"

Timer::Timer()
{
    channels[0].prev = nullptr;
    channels[1].prev = &channels[0];
    channels[2].prev = &channels[1];
    channels[3].prev = &channels[2];

    channels[0].next = &channels[1];
    channels[1].next = &channels[2];
    channels[2].next = &channels[3];
    channels[3].next = nullptr;
}
