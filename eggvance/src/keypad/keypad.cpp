#include "keypad.h"

#include "core/core.h"

Keypad::Keypad(Core& core)
    : core(core)
{

}

void Keypad::update()
{
    u16 previous = io.input.value;
    io.input.value = core.context.input.state() & KeypadIo::Input::kMask;

    if (previous != io.input.value && io.control.irq)
    {
        bool interrupt = io.control.logic
            ? (~io.input.value == io.control.mask)
            : (~io.input.value &  io.control.mask);

        if (interrupt)
            core.irqh.request(kIrqKeypad);
    }
}
