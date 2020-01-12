#include "synchronizer.h"

#include "microclock.h"
#include "common/constants.h"

namespace micro = micro_clock;

Synchronizer::Synchronizer()
{
    setFps(REFRESH_RATE);
}

void Synchronizer::setFps(double fps)
{
    frame_delta = 0;
    frame_begin = micro::now();
    frame_duration = static_cast<u64>(1.0e6 / fps);
}

void Synchronizer::beginFrame()
{
    frame_begin = micro::now();
}

void Synchronizer::endFrame()
{
    frame_delta += micro::now() - frame_begin;

    if (frame_delta < static_cast<s64>(frame_duration))
        frame_delta += sleep(static_cast<u32>(frame_duration - frame_delta));

    frame_delta -= frame_duration;
}

u64 Synchronizer::sleep(u32 us)
{
    u64 now = micro::now();
    micro::sleep(us);
    return micro::now() - now;
}
