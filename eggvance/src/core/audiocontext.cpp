#include "audiocontext.h"

#include "base/panic.h"

AudioContext::~AudioContext()
{
    deinit();
}

void AudioContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        panic("Cannot init audio context {}", SDL_GetError());

    SDL_AudioSpec want = {};
    SDL_AudioSpec have = {};

    want.freq     = 44100;
    want.samples  = 1024;
    want.format   = AUDIO_F32SYS;
    want.channels = 2;
    want.userdata = this;
    want.callback = callback;

    if (!(device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0)))
        panic("Cannot init audio device {}", SDL_GetError());

    if (!(stream = SDL_NewAudioStream(AUDIO_S16, 2, 32768, have.format, have.channels, have.freq)))
        panic("Cannot init audio stream {}", SDL_GetError());
}

void AudioContext::pause()
{
    SDL_PauseAudioDevice(device, true);
}

void AudioContext::unpause()
{
    SDL_PauseAudioDevice(device, false);
}

void AudioContext::write(s16 left, s16 right)
{
    constexpr std::size_t kSecond = 44100 * 2 * sizeof(float);

    std::lock_guard lock(mutex);
    if (SDL_AudioStreamAvailable(stream) < kSecond / 8)
    {
        s16 sample[] = { left, right };
        SDL_AudioStreamPut(stream, &sample, sizeof(sample));
    }
}

void AudioContext::clear()
{
    std::lock_guard lock(mutex);
    SDL_AudioStreamClear(stream);
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

    if (gotten >= length)
        return;

    auto f_sample = 0.0f;
    auto f_stream = reinterpret_cast<float*>(stream);
    auto f_gotten = gotten / sizeof(float);
    auto f_length = length / sizeof(float);

    if (f_gotten)
        f_sample = f_stream[f_gotten - 1];

    std::fill(f_stream + f_gotten, f_stream + f_length, f_sample);
}

void AudioContext::deinit()
{

    if (SDL_WasInit(SDL_INIT_AUDIO))
    {
        SDL_FreeAudioStream(stream);
        SDL_CloseAudioDevice(device);

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}
