#pragma once

#if defined (_MSC_VER) || defined (__EMSCRIPTEN__)
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
