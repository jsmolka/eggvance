#include "context.h"

Context::~Context()
{
    deinit();
}

void Context::init()
{
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
