#pragma once

#ifdef _MSC_VER
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

#include "platform/audiodevice.h"

class SDLAudioDevice : public AudioDevice
{
public:
    ~SDLAudioDevice();

    void init() override;
    void deinit() override;
};
