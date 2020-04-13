#pragma once

#include "common/defines.h"
#include "platform/audiodevice.h"

#if COMPILER_MSVC || COMPILER_EMSCRIPTEN
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

class SDLAudioDevice : public AudioDevice
{
public:
    ~SDLAudioDevice();

    void init() override;
    void deinit() override;
};
