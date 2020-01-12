#include "sdlaudiodevice.h"

#include <stdexcept>

SDLAudioDevice::~SDLAudioDevice()
{
    deinit();
}

void SDLAudioDevice::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw std::runtime_error("Cannot init audio device");
}

void SDLAudioDevice::deinit()
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
