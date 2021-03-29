#pragma once

#include <chrono>
#include <optional>

#include "base/int.h"

class FrameCounter
{
public:
    FrameCounter();
    FrameCounter& operator++();

    void reset();

    std::optional<double> fps();

private:
    using Clock = std::chrono::high_resolution_clock;
    using Time  = std::chrono::high_resolution_clock::time_point;

    Time begin;
    uint count;
};
