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
    case 0: updateData(); return control.readByte<0>();
    case 1: updateData(); return control.readByte<1>();
    case 2: return control.readByte<2>();
    case 3: return control.readByte<3>();
    }
    return 0;
}

void Timer::writeByte(int index, u8 byte)
{
    updateData();
    switch (index)
    {
    case 0: control.writeByte<0>(byte); break;
    case 1: control.writeByte<1>(byte); break;
    case 2: 
    {
        int enabled = control.enabled;
        control.writeByte<2>(byte);
        if (!enabled && control.enabled)
            control.data = control.initial;
        break;
    }
    case 3: control.writeByte<3>(byte); break;
    }
    calculate();
}

void Timer::runInternal(int cycles)
{
    counter += cycles;

    // Todo: check if multiple overflows?
    if (counter >= overflow)
    {
        // Todo: check if timer is enabled?
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

        counter = control.initial + (counter % overflow);
    }
}

void Timer::updateData()
{
    control.data = counter / prescalers[control.prescaler];
}

void Timer::calculate()
{
    overflow = prescalers[control.prescaler] * 0x1'0000;
    counter  = prescalers[control.prescaler] * control.data;
}
