#include "audiocontext.h"

#include "base/sdl2.h"

void AudioContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw std::runtime_error("Cannot init audio context");
}

void AudioContext::deinit()
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
