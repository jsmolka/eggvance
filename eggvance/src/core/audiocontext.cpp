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

    if (SDL_OpenAudio(&want, &have) < 0)
        panic("Cannot init audio device {}", SDL_GetError());

    stream = SDL_NewAudioStream(AUDIO_S16, 2, 0x8000, have.format, have.channels, have.freq);

    if (!stream)
        panic("Cannot init audio stream {}", SDL_GetError());

    SDL_PauseAudio(0);
}

void AudioContext::callback(void* userdata, u8* stream, int length)
{
    auto self = reinterpret_cast<AudioContext*>(userdata);

    int gotten = 0;
    {
        std::lock_guard<std::mutex> lock(self->mutex);
        if (SDL_AudioStreamAvailable(self->stream))
            gotten = SDL_AudioStreamGet(self->stream, stream, length);
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

void AudioContext::deinit()
{

    if (SDL_WasInit(SDL_INIT_AUDIO))
    {
        if (stream)
            SDL_FreeAudioStream(stream);

        SDL_CloseAudio();

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}
