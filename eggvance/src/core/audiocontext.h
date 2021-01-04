#pragma once

#include <mutex>

#include "base/int.h"
#include "base/sdl2.h"
#include "base/stereosample.h"
#include "base/ringbuffer.h"

#define RESAMPLE_AUDIO 1

class AudioContext
{
public:
    static constexpr uint kSampleRate = 32768;
    static constexpr uint kSamples    = 1024;

    using Sample = StereoSample<s16>;

    ~AudioContext();

    void init();
    void pause();
    void resume();
    void write(const Sample& sample);
    void clear();

private:
    static void callback(void* userdata, u8* stream, int length);

    void deinit();

    std::mutex mutex;
    SDL_AudioDeviceID device = 0;
    #if RESAMPLE_AUDIO
    SDL_AudioStream* stream = nullptr;
    #else
    RingBuffer<Sample, kSamples> buffer;
    #endif
};

inline AudioContext audio_ctx;
