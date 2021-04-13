#include "framecounter.h"

FrameCounter::FrameCounter()
{
    reset();
}

FrameCounter& FrameCounter::operator++()
{
    count++;
    return *this;
}

void FrameCounter::reset()
{
    count = 0;
    begin = Clock::now();
    queue_reset = false;
}

void FrameCounter::queueReset()
{
    queue_reset = true;
}

std::optional<double> FrameCounter::fps()
{
    using Seconds = std::chrono::duration<double>;

    if (queue_reset)
    {
        reset();
        return std::nullopt;
    }

    auto delta = Clock::now() - begin;
    if ( delta < std::chrono::seconds(1))
        return std::nullopt;

    double fps = count / std::chrono::duration_cast<Seconds>(delta).count();

    reset();

    return fps;
}
