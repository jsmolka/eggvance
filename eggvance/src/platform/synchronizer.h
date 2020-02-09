#pragma once

#include <chrono>

class Synchronizer
{
public:
    Synchronizer();

    void setFps(double fps);
    void beginFrame();
    void endFrame();

private:
    std::chrono::nanoseconds delta;
    std::chrono::nanoseconds frame;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
};
