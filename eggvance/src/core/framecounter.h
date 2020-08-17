#pragma once

#include <chrono>

#include "base/integer.h"

class FrameCounter
{
public:
    FrameCounter();

    FrameCounter& operator++();

    bool queryFps(double& value);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    uint count;
};
