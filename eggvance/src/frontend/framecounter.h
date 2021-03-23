#pragma once

#include <chrono>
#include <optional>

#include "base/int.h"

template<typename Clock = std::chrono::high_resolution_clock>
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
    using Time = typename Clock::time_point;

    Time begin;
    uint count;
};
