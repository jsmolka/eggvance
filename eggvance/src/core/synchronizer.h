#pragma once

#include <chrono>
#include <functional>

class Synchronizer
{
public:
    Synchronizer(double fps);

    void sync(const std::function<void(void)>& frame);

private:
    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::high_resolution_clock::duration;

    Duration delta_have;
    Duration delta_want;
};
