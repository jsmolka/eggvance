#include "keypad.h"

#include "arm/arm.h"
#include "frontend/inputcontext.h"

void Keypad::update()
{
    input = ~input_ctx.state();

    checkInterrupt();
}

void Keypad::checkInterrupt()
{
    enum class Logic { Any, All };

    if (!control.irq)
        return;

    uint mask = ~input & input.mask;

    if (control.logic == Logic::Any ? (mask & control.mask) : (mask == control.mask))
        arm.raise(Irq::Keypad);
}
