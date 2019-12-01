#pragma once

#include <memory>

class AudioDevice
{
public:
    virtual ~AudioDevice() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
};

extern std::shared_ptr<AudioDevice> audio_device;
