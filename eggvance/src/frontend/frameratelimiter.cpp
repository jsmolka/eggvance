#include "frameratelimiter.h"

FrameRateLimiter::FrameRateLimiter()
{
    setMultiplier(1);
}

void FrameRateLimiter::reset()
{
    accumulated = Duration(0);
    queue_reset = false;
}

void FrameRateLimiter::queueReset()
{
    queue_reset = true;
}

void FrameRateLimiter::setMultiplier(double multiplier)
{
    constexpr auto kRefreshRate = 59.737;

    this->multiplier = multiplier;

    setFps(kRefreshRate * multiplier);
}

bool FrameRateLimiter::isFastForward() const
{
    return multiplier > 1;
}

void FrameRateLimiter::setFps(double fps)
{
    frame_delta = Duration(Duration::rep(Duration::period::den / fps));

    queueReset();
}
