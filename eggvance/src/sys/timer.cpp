#include "timer.h"

#include "arm/arm.h"

Timer::Timer(int id)
{
    this->id = id;
}

void Timer::reset()
{
    cycles          = 0;
    cycles_max      = 0;
    cycles_inital   = 0;
    cycles_overflow = 0;

    control.reset();
    data.reset();
}

void Timer::run(u64 accumulated)
{
    cycles += accumulated;

    if (cycles >= cycles_overflow)
    {
        if (next && next->control.cascade)
        {
            next->run(cycles / cycles_overflow);
        }

        if (control.irq)
        {
            static constexpr Interrupt flags[4] = {
                Interrupt::Timer0,
                Interrupt::Timer1,
                Interrupt::Timer2,
                Interrupt::Timer3
            };
            arm.request(flags[id]);
        }

        cycles %= cycles_overflow;
    }
}

void Timer::start()
{
    cycles = 0;
    cycles_max = u64(control.prescaler) * 0x1'0000;
    cycles_inital = u64(control.prescaler) * data.initial;
    cycles_overflow = cycles_max - cycles_inital;
}

void Timer::update()
{
    data.data = static_cast<u16>(cycles / control.prescaler);
}

bool Timer::canChange() const
{
    return control.enabled || (control.cascade && inActiveCascadeChain());
}

bool Timer::canCauseInterrupt() const
{
    if (!control.irq)
        return false;

    if (control.cascade)
        return inActiveCascadeChain();
    else
        return control.enabled;
}

u64 Timer::interruptsAfter() const
{
    if (control.cascade)
    {
        if (!prev) 
            return 0xFFFFFFFFull;

        u64 remaining = 1;

        const auto* p = this;
        while (p)
        {
            remaining *= p->cycles_overflow;
            remaining -= p->cycles;

            if (!p->control.cascade)
                break;
            p = p->prev;
        }
        return remaining;
    }
    else
    {
        return cyclesRemaining();
    }
}

bool Timer::inActiveCascadeChain() const
{
    if (!control.cascade)
        return false;

    Timer* p = prev;
    while (p && p->control.cascade)
        p = p->prev;

    return p && p->control.enabled;
}

u64 Timer::cyclesRemaining() const
{
    return cycles_overflow - cycles;
}
