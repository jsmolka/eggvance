#pragma once

#include "platform.h"
#include "devices/audiodevice.h"

class SDLAudioDevice : public AudioDevice
{
public:
    ~SDLAudioDevice();

    void init() override;
    void deinit() override;
};
