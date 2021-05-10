#include "keypad.h"

#include "arm/arm.h"
#include "frontend/inputcontext.h"

void Keypad::update()
{
    uint previous = input;
    input = ~input_ctx.state();

    if (input != previous)
        checkInterrupt();
}

void Keypad::checkInterrupt()
{
    enum class Logic { Any, All };

    if (!control.irq)
        return;

    uint mask = ~input & control.mask;

    if (control.logic == Logic::Any ? mask : mask == control.mask)
        arm.raise(Irq::Keypad);
}
