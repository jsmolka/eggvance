#pragma once

#include <mutex>

#include "base/int.h"
#include "base/sdl2.h"

class AudioContext
{
public:
    ~AudioContext();

    void init();

    template<typename T>
    void write(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        SDL_AudioStreamPut(stream, reinterpret_cast<const void*>(&value), sizeof(T));
    }

private:
    static void callback(void* userdata, u8* stream, int length);

    void deinit();

    std::mutex mutex;
    SDL_AudioStream* stream = nullptr;
};

inline AudioContext audio_ctx;
