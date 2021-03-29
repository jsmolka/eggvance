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
}

std::optional<double> FrameCounter::fps()
{
    using Seconds = std::chrono::duration<double>;

    auto delta = Clock::now() - begin;
    if ( delta < std::chrono::seconds(1))
        return std::nullopt;

    double fps = count / std::chrono::duration_cast<Seconds>(delta).count();

    reset();

    return fps;
}
