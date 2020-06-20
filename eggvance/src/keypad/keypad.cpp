#include "keypad.h"

#include "irq/irqh.h"
#include "platform/inputdevice.h"

Keypad keypad;

void Keypad::process()
{
    u16 prev = io.input;
    io.input = input_device->state();

    if (prev != io.input && io.control.irq)
    {
        bool interrupt = io.control.logic
            ? (~io.input == io.control.mask)
            : (~io.input &  io.control.mask);

        if (interrupt)
            irqh.request(kIrqKeypad);
    }
}
