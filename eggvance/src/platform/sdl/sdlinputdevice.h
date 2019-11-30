#pragma once

#include "devices/inputdevice.h"

class SDLInputDevice : public InputDevice
{
public:
    ~SDLInputDevice();

    void init() override;
    void deinit() override;
};
