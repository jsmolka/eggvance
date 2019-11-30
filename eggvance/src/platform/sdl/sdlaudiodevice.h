#pragma once

#include <SDL2/SDL.h>

#include "devices/audiodevice.h"

class SDLAudioDevice : public AudioDevice
{
public:
    ~SDLAudioDevice();

    void init() override;
    void deinit() override;
};
