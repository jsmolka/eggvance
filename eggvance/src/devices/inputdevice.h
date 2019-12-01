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
    enum Shift
    {
        SHIFT_A      = 0,
        SHIFT_B      = 1,
        SHIFT_SELECT = 2,
        SHIFT_START  = 3,
        SHIFT_RIGHT  = 4,
        SHIFT_LEFT   = 5,
        SHIFT_UP     = 6,
        SHIFT_DOWN   = 7,
        SHIFT_R      = 8,
        SHIFT_L      = 9
    };
};
