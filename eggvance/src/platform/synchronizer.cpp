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
    frame = nanoseconds(nanoseconds::rep(nanoseconds::period::den / fps));
    begin = high_resolution_clock::now();
}

void Synchronizer::beginFrame()
{
    begin = high_resolution_clock::now();
}

void Synchronizer::endFrame()
{
    delta += high_resolution_clock::now() - begin;

    if (delta < frame)
    {
        auto then = high_resolution_clock::now();
        std::this_thread::sleep_for(frame - delta);
        delta += high_resolution_clock::now() - then;
    }
    delta -= frame;
}
