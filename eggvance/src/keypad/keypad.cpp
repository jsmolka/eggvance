#include "keypad.h"

#include "core/core.h"
#include "irq/irqh.h"

Keypad keypad;

void Keypad::update()
{
    u16 previous = io.input.value;
    io.input.value = g_core.context.input.state() & KeypadIo::Input::kMask;

    if (previous != io.input.value && io.control.irq)
    {
        bool interrupt = io.control.logic
            ? (~io.input.value == io.control.mask)
            : (~io.input.value &  io.control.mask);

        if (interrupt)
            irqh.request(kIrqKeypad);
    }
}
