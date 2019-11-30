#pragma once

class InputDevice
{
public:
    virtual ~InputDevice() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
};
