#include "keypad.h"

#include "irq/irqh.h"
#include "platform/inputdevice.h"

Keypad keypad;

void Keypad::update()
{
    u16 previous = io.input.value;
    io.input.value = input_device->state() & KeypadIo::Input::kMask;

    if (previous != io.input.value && io.control.irq)
    {
        bool interrupt = io.control.logic
            ? (~io.input.value == io.control.mask)
            : (~io.input.value &  io.control.mask);

        if (interrupt)
            irqh.request(kIrqKeypad);
    }
}
