#include "framecounter.h"

#include "microclock.h"

namespace micro = micro_clock;

FrameCounter::FrameCounter()
{
    start = micro::now();
    count = 0;
}

FrameCounter& FrameCounter::operator++()
{
    count++;
    return *this;
}

bool FrameCounter::fps(double& value)
{
    u64 delta = micro::now() - start;
    if (delta >= 1000000)
    {
        value = 1000000.0 / static_cast<double>(delta) * static_cast<double>(count);
        start = micro::now();
        count = 0;
        return true;
    }
    return false;
}
