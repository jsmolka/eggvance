#include "frameratelimiter.h"

#include "base/constants.h"

bool FrameRateLimiter::isFastForward()
{
    return fps > kRefreshRate;
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

void FrameRateLimiter::setFps(double fps)
{
    this->fps = fps;

    frame_delta = Duration(Duration::rep(Duration::period::den / fps));

    queueReset();
}
