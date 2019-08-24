#include "timercontrol.h"

void TimerControl::reset()
{
    prescaler = 0;
    cascade   = 0;
    irq       = 0;
    enabled   = 0;
    data      = 0;
    initial   = 0;
    counter   = 0;
}
