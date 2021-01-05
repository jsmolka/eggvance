#pragma once

#include <mutex>

#include "base/int.h"
#include "base/sdl2.h"

class AudioContext
{
public:
    ~AudioContext();

    void init();
    void pause();
    void resume();
    void write(s16 left, s16 right);
    void clear();

private:
    static void callback(void* data, u8* stream, int length);

    void deinit();

    std::mutex mutex;
    SDL_AudioDeviceID device = 0;
    SDL_AudioStream*  stream = nullptr;
};

inline AudioContext audio_ctx;
