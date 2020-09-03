#include "keypad.h"

#include "arm/arm.h"
#include "core/inputcontext.h"

void Keypad::update()
{
    u16 previous = io.input.value;
    io.input.value = input_ctx.state() & KeypadIo::Input::kMask;

    if (previous != io.input.value && io.control.irq)
    {
        bool interrupt = io.control.logic
            ? (~io.input.value == io.control.mask)
            : (~io.input.value &  io.control.mask);

        if (interrupt)
            arm.raise(kIrqKeypad);
    }
}
