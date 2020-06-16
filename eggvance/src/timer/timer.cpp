#include "timer.h"

#include "interrupt/irqhandler.h"

constexpr auto kTimerOverflow = 0x1'0000;

Timer::Timer(int id)
    : id(id)
{

}

void Timer::init()
{
    counter  = 0;
    initial  = io.data.initial;
    overflow = io.ctrl.prescale * (kTimerOverflow - initial);
}

void Timer::update()
{
    counter  = io.ctrl.prescale * (io.data.counter - initial);
    overflow = io.ctrl.prescale * (kTimerOverflow - initial);
}

void Timer::run(int cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->io.ctrl.cascade)
            next->run(counter / overflow);

        if (io.ctrl.irq)
            irqh.request(kIrqTimer << id);

        counter %= overflow;
        initial  = io.data.initial;
        overflow = io.ctrl.prescale * (kTimerOverflow - initial);
    }
    io.data.counter = counter / io.ctrl.prescale + initial;
}

int Timer::nextOverflow() const
{
    return overflow - counter;
}
