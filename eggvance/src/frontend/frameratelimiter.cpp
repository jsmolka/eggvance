#include "frameratelimiter.h"

inline constexpr auto kRefreshRate = 59.737;

FrameRateLimiter::FrameRateLimiter()
{
    setFps(kRefreshRate);
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

void FrameRateLimiter::setFastForward(double fast_forward)
{
    this->fast_forward = fast_forward;

    setFps(kRefreshRate * fast_forward);
}

bool FrameRateLimiter::isFastForward() const
{
    return fast_forward > 1;
}

void FrameRateLimiter::setFps(double fps)
{
    frame_delta = Duration(Duration::rep(Duration::period::den / fps));

    queueReset();
}
