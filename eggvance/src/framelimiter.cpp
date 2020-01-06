#include "framelimiter.h"

#include "clock.h"

FrameLimiter::FrameLimiter(double fps)
{
    setFPS(fps);
}

void FrameLimiter::setFPS(double fps)
{
    reset();

    frame = static_cast<u64>(1.0e6 / fps);
}

void FrameLimiter::beginFrame()
{
    begin = clock::now();
}

void FrameLimiter::endFrame()
{
    if (begin == 0)
        return;

    delta += clock::now() - begin;
    if (delta < static_cast<s64>(frame))
    {
        u64 sleep_begin = clock::now();
        clock::sleep(static_cast<u32>(frame - delta));
        delta += clock::now() - sleep_begin;
    }
    delta -= frame;
}

void FrameLimiter::reset()
{
    frame = 0;
    begin = 0;
    delta = 0;
}
