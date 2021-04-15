#pragma once

#include <mutex>
#include <shell/array.h>

#include "sdl2.h"
#include "base/int.h"

class AudioContext
{
public:
    using Samples = shell::array<s16, 2>;

    ~AudioContext();

    void init();
    void pause();
    void unpause();
    void write(Samples samples);

private:
    static void callback(void* data, u8* stream, int length);

    std::mutex mutex;
    SDL_AudioDeviceID device = 0;
    SDL_AudioStream*  stream = nullptr;
};

inline AudioContext audio_ctx;
