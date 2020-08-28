#include "framecounter.h"

#include <eggcpt/utility.h>

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
        eggcpt::reconstruct(*this);
        return fps;
    }
    return std::nullopt;
}
