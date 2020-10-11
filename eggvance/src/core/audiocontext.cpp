#include "audiocontext.h"

#include "base/sdl2.h"
#include "base/utility.h"

AudioContext::~AudioContext()
{
    deinit();
}

void AudioContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        exit("Cannot init audio context");
}

void AudioContext::deinit()
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
