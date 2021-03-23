#include "keypad.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "frontend/inputcontext.h"

void Keypad::update()
{
    input = ~input_ctx.state();

    checkInterrupt();
}

void Keypad::checkInterrupt()
{
    enum Condition
    {
        kConditionAny,
        kConditionAll
    };

    if (!control.irq)
        return;

    uint mask = ~input & input.mask;

    if (control.cond == kConditionAny ? (mask & control.mask) : (mask == control.mask))
        arm.raise(kIrqKeypad);
}
