#include "timer.h"

Timer::Timer()
{
    channels[0].next = &channels[1];
    channels[1].next = &channels[2];
    channels[2].next = &channels[3];
    channels[3].next = nullptr;
}
