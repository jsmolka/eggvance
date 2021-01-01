#pragma once

#include "base/sdl2.h"

class AudioContext
{
public:
    ~AudioContext();

    void init();
    void open(void* userdata, SDL_AudioCallback callback);

private:
    void deinit();
    void close();

    SDL_AudioDeviceID device = 0;
};

inline AudioContext audio_ctx;
