#include "context.h"

#include "base/sdl2.h"

Context::~Context()
{
    deinit();
}

void Context::init()
{
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    audio.init();
    input.init();
    video.init();
}

void Context::deinit()
{
    audio.deinit();
    input.deinit();
    video.deinit();
}
