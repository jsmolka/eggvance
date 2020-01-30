#include "keypad.h"

#include "irqhandler.h"
#include "platform/inputdevice.h"

Keypad keypad;

void Keypad::reset()
{
    io.keycnt.reset();
    io.keyinput.reset();
}

void Keypad::process()
{
    u16 previous = io.keyinput;

    io.keyinput = input_device->state();

    if (previous != io.keyinput && io.keycnt.irq)
    {
        bool interrupt = io.keycnt.logic
            ? (~io.keyinput == io.keycnt.mask)
            : (~io.keyinput &  io.keycnt.mask);

        if (interrupt)
            irqh.request(kIrqKeypad);
    }
}
