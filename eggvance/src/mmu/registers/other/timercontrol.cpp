#include "timercontrol.h"

TimerControl::TimerControl(u16& data)
    : Register<u16>(data)
    , prescaler(data)
    , cascade(data)
    , irq(data)
    , enabled(data)
{

}
