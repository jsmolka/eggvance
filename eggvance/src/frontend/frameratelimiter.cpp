#include "frameratelimiter.h"

FrameRateLimiter::FrameRateLimiter(double fps)
{
    accumulated = Duration(0);
    frame_delta = Duration(Duration::rep(Duration::period::den / fps));
}

void FrameRateLimiter::reset()
{
    accumulated = Duration(0);
}
