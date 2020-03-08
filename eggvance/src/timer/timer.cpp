#include "timer.h"

#include "system/irqhandler.h"

constexpr uint limit = 0x1'0000;

Timer::Timer(uint id)
{
    this->id = id;
}

void Timer::run(uint cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->io.control.cascade)
            next->run(counter / overflow);

        if (io.control.irq)
            irqh.request(
                static_cast<Irq>(
                    static_cast<uint>(Irq::Timer) << id));

        counter %= overflow;
        reload   = io.data.initial;
        overflow = prescale(limit - reload);
    }
    io.data.counter = counter / io.control.prescaler + reload;
}

void Timer::start()
{
    counter  = 0;
    reload   = io.data.initial;
    overflow = prescale(limit - reload);
}

void Timer::update()
{
    counter  = prescale(io.data.counter - reload);
    overflow = prescale(limit - reload);
}

uint Timer::nextOverflow() const
{
    return overflow - counter;
}

uint Timer::prescale(uint value) const
{
    return io.control.prescaler * value;
}
