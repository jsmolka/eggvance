#pragma once

#include "common/integer.h"

class InputDevice
{
public:
    virtual ~InputDevice() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void poll(u16& state) = 0;

protected:
    enum Button
    {
        BTN_A      = 0,
        BTN_B      = 1,
        BTN_SELECT = 2,
        BTN_START  = 3,
        BTN_RIGHT  = 4,
        BTN_LEFT   = 5,
        BTN_UP     = 6,
        BTN_DOWN   = 7,
        BTN_R      = 8,
        BTN_L      = 9
    };
};
