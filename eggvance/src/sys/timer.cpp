#include "timer.h"

#include "arm/arm.h"

void Timer::reset()
{
    control.reset();

    counter  = 0;
    overflow = 0;
}

void Timer::run(int cycles)
{
    if (!control.enabled || control.cascade)
        return;

    runInternal(cycles);
}

u8 Timer::readByte(int index)
{
    switch (index)
    {
    case 0:
    case 1:
        updateData();
        return data.readByte(index);

    case 2:
    case 3:
        return control.readByte(index - 2);
    }
    return 0;
}

void Timer::writeByte(int index, u8 byte)
{
    updateData();

    int enabled = control.enabled;
    switch (index)
    {
    case 0:
    case 1:
        data.writeByte(index, byte);
        break;
    case 2:
    case 3:
        control.writeByte(index - 2, byte);
        break;
    }
    if (!enabled && control.enabled)
        data.data = data.initial;

    calculate();
}

void Timer::runInternal(int cycles)
{
    counter += cycles;

    // Todo: check if multiple overflows?
    if (counter >= overflow)
    {
        if (next && next->control.cascade)
            next->runInternal(1);

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

        counter = prescalers[control.prescaler] * data.initial + (counter % overflow);
    }
}

void Timer::updateData()
{
    data.data = counter / prescalers[control.prescaler];
}

void Timer::calculate()
{
    overflow = prescalers[control.prescaler] * 0x1'0000;
    counter  = prescalers[control.prescaler] * data.data;
}
