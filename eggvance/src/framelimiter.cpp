#include "framelimiter.h"

#include "microclock.h"

namespace micro = micro_clock;

FrameLimiter::FrameLimiter()
{
    frame_time  = 0;
    frame_beg   = 0;
    frame_end   = 0;
    frame_delta = 0;
}

void FrameLimiter::setFPS(double fps)
{
    frame_time = 1.0e6 / fps;
}

void FrameLimiter::begin()
{
    frame_beg = micro::now();
}

void FrameLimiter::end()
{
    frame_end = micro::now();

    frame_delta += frame_end - frame_beg;

    if (frame_delta < static_cast<s64>(frame_time))
    {
        u64 sleep_begin = micro::now();
        micro::sleep(static_cast<u32>(frame_time - frame_delta));
        frame_delta += micro::now() - sleep_begin;
    }
    frame_delta -= frame_time;
}
