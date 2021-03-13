#include "gamepad.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "core/inputcontext.h"

void Gamepad::update()
{
    input.value = ~input_ctx.state() & KeyInput::kMask;

    tryRaise();
}

void Gamepad::tryRaise()
{
    if (control.irq && control.raisesIrq(input))
        arm.raise(kIrqGamepad);
}
