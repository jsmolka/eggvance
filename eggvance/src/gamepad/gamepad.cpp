#include "gamepad.h"

#include "arm/arm.h"
#include "core/inputcontext.h"

void Gamepad::poll()
{
    u16 previous = input.value;
    input.value = ~input_ctx.state() & KeyInput::kMask;

    if (previous != input.value
            && control.irq
            && control.raisesIrq(input))
        arm.raise(kIrqGamepad);
}
