#pragma once

class AudioDevice
{
public:
    virtual ~AudioDevice() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
};
