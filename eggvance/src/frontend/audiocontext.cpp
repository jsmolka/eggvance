#include "audiocontext.h"

#include <shell/errors.h>

#include "base/config.h"
#include "base/constants.h"

inline constexpr auto kWantFrequency = 44100;
inline constexpr auto kWantChannels  = 2;

AudioContext::~AudioContext()
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
    {
        SDL_FreeAudioStream(stream);
        SDL_CloseAudioDevice(device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

void AudioContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw shell::Error("Cannot init audio context: {}", SDL_GetError());

    SDL_AudioSpec want = {};
    SDL_AudioSpec have = {};

    want.freq     = kWantFrequency;
    want.samples  = 1024;
    want.format   = AUDIO_F32SYS;
    want.channels = kWantChannels;
    want.userdata = this;
    want.callback = callback;

    if (!(device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0)))
        throw shell::Error("Cannot init audio device: {}", SDL_GetError());

    if (!(stream = SDL_NewAudioStream(AUDIO_S16, 2, kSampleRate, have.format, have.channels, have.freq)))
        throw shell::Error("Cannot init audio stream: {}", SDL_GetError());
}

void AudioContext::pause()
{
    std::lock_guard lock(mutex);
    SDL_AudioStreamClear(stream);
    SDL_PauseAudioDevice(device, true);
}

void AudioContext::unpause()
{
    SDL_PauseAudioDevice(device, false);
}

void AudioContext::write(Samples samples)
{
    constexpr auto kSecond = kWantFrequency * kWantChannels * sizeof(float);

    std::lock_guard lock(mutex);
    if (SDL_AudioStreamAvailable(stream) < kSecond / 8)
    {
        samples[0] = static_cast<s16>(config.volume * static_cast<double>(samples[0]));
        samples[1] = static_cast<s16>(config.volume * static_cast<double>(samples[1]));

        SDL_AudioStreamPut(stream, samples.data(), sizeof(Samples));
    }
}

void AudioContext::callback(void* data, u8* stream, int length)
{
    AudioContext& self = *reinterpret_cast<AudioContext*>(data);

    int gotten = 0;
    {
        std::lock_guard lock(self.mutex);
        if (SDL_AudioStreamAvailable(self.stream))
            gotten = SDL_AudioStreamGet(self.stream, stream, length);
    }

    if (gotten == -1)
    {
        std::memset(stream, 0, length);
        return;
    }

    if (gotten < length)
    {
        auto f_sample = 0.0f;
        auto f_stream = reinterpret_cast<float*>(stream);
        auto f_gotten = gotten / sizeof(float);
        auto f_length = length / sizeof(float);

        if (f_gotten)
            f_sample = f_stream[f_gotten - 1];

        std::fill(f_stream + f_gotten, f_stream + f_length, f_sample);
    }
}
