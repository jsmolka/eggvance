#include "timer.h"

#include "interrupt/irqhandler.h"

constexpr uint kLimit = 0x1'0000;

Timer::Timer(uint id)
    : id(id) {}

void Timer::start()
{
    counter  = 0;
    initial  = io.data.initial;
    overflow = io.ctrl.prescale * (kLimit - initial);
}

void Timer::update()
{
    counter  = io.ctrl.prescale * (io.data.counter - initial);
    overflow = io.ctrl.prescale * (kLimit - initial);
}

void Timer::run(uint cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->io.ctrl.cascade)
            next->run(counter / overflow);

        if (io.ctrl.irq)
            irqh.request(
                static_cast<IRQ>(
                    static_cast<uint>(IRQ::Timer) << id));

        counter %= overflow;
        initial  = io.data.initial;
        overflow = io.ctrl.prescale * (kLimit - initial);
    }
    io.data.counter = counter / io.ctrl.prescale + initial;
}

uint Timer::nextOverflow() const
{
    return overflow - counter;
}
