#pragma once

#include <memory>

#include "device.h"

class InputDevice : public Device
{
public:
    virtual int state() = 0;

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

extern std::shared_ptr<InputDevice> input_device;
