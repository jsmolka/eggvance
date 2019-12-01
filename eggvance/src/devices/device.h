#pragma once

class Device
{
public:
    virtual ~Device() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
};
