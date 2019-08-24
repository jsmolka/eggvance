#include "waitcontrol.h"

void WaitControl::WaitState::reset()
{
    n = 0;
    s = 0;
}

void WaitControl::reset()
{
    sram = 0;
    ws0.reset();
    ws1.reset();
    ws2.reset();
}
