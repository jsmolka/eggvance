#pragma once

#include <chrono>
#include <functional>

class Synchronizer
{
public:
    using Frame = std::function<void(void)>;

    Synchronizer();

    void setFps(double fps);
    void synchronize(const Frame& frame);

private:
    std::chrono::nanoseconds delta;
    std::chrono::nanoseconds duration;
};
