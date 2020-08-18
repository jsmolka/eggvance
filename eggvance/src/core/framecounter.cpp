#include "framecounter.h"

FrameCounter::FrameCounter()
{
    count = 0;
    begin = Clock::now();
}

FrameCounter& FrameCounter::operator++()
{
    count++;
    return *this;
}

std::optional<double> FrameCounter::fps()
{
    auto delta = Clock::now() - begin;
    if (delta >= std::chrono::seconds(1))
    {
        double fps = count / std::chrono::duration<double>(delta).count();
        *this = FrameCounter();
        return fps;
    }
    return std::nullopt;
}
