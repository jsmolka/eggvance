#include "keypad.h"

#include "arm/arm.h"
#include "devices/devices.h"

Keypad keypad;

void Keypad::reset()
{
    io.keycnt.reset();
    io.keyinput.reset();
}

void Keypad::poll()
{
    u16 previous = io.keyinput;

    input_device->poll(io.keyinput.value);

    if (previous != io.keyinput)
    {
        if (io.keycnt.irq)
        {
            bool interrupt = io.keycnt.logic
                ? (~io.keyinput == io.keycnt.mask)
                : (~io.keyinput &  io.keycnt.mask);

            if (interrupt)
            {
                arm.request(Interrupt::Keypad);
            }
        }
    }
}
