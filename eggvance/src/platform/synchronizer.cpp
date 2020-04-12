#include "synchronizer.h"

#include <thread>

#include "common/constants.h"

using namespace std::chrono;

Synchronizer::Synchronizer()
{
    setFps(REFRESH_RATE);
}

void Synchronizer::setFps(double fps)
{
    delta = nanoseconds(0);
    duration = nanoseconds(nanoseconds::rep(nanoseconds::period::den / fps));
}

void Synchronizer::synchronize(const Frame& frame)
{
    auto beg = high_resolution_clock::now();
    frame();
    auto end = high_resolution_clock::now();
    
    delta += end - beg;
    if (delta < duration)
    {
        std::this_thread::sleep_for(duration - delta);
        delta += high_resolution_clock::now() - end;
    }
    delta -= duration;
}
