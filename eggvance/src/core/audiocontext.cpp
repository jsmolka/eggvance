#include "audiocontext.h"

#include "base/panic.h"
#include "base/sdl2.h"

AudioContext::~AudioContext()
{
    deinit();
}

void AudioContext::init()
{
    close();

    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        panic("Cannot init audio context {}", SDL_GetError());
}

void AudioContext::open(void* userdata, SDL_AudioCallback callback)
{
    close();

    SDL_AudioSpec want = {};
    SDL_AudioSpec have = {};

    want.freq     = 48000;
    want.samples  = 1024;
    want.format   = AUDIO_F32SYS;
    want.channels = 1;
    want.userdata = userdata;
    want.callback = callback;

    device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if (device == 0)
        panic("Cannot init audio device {}", SDL_GetError());

    if (want.format != have.format)
        panic("Bad audio format");

    if (want.channels != have.channels)
        panic("Bad audio channels");

    SDL_PauseAudioDevice(device, 0);
}

void AudioContext::deinit()
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioContext::close()
{
    if (device != 0)
    {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
}
