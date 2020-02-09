#include "framecounter.h"

using namespace std::chrono;

FrameCounter::FrameCounter()
{
    begin = high_resolution_clock::now();
    count = 0;
}

FrameCounter& FrameCounter::operator++()
{
    count++;
    return *this;
}

bool FrameCounter::queryFps(double& value)
{
    auto delta = high_resolution_clock::now() - begin;
    if (delta >= seconds(1))
    {
        value = count / duration<double>(delta).count();
        begin = high_resolution_clock::now();
        count = 0;

        return true;
    }
    return false;
}
