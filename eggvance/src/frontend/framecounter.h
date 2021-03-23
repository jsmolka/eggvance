#pragma once

#include <chrono>
#include <optional>

#include "base/int.h"

class FrameCounter
{
public:
    FrameCounter()
    {
        count = 0;
        begin = Clock::now();
    }

    FrameCounter& operator++()
    {
        count++;
        return *this;
    }

    std::optional<double> fps()
    {
        using Seconds = std::chrono::duration<double>;

        auto delta = Clock::now() - begin;
        if ( delta < std::chrono::seconds(1))
            return std::nullopt;

        double fps = count / std::chrono::duration_cast<Seconds>(delta).count();

        count = 0;
        begin = Clock::now();
        
        return fps;
    }

private:
    using Clock = std::chrono::high_resolution_clock;
    using Time  = std::chrono::high_resolution_clock::time_point;

    Time begin;
    uint count;
};
