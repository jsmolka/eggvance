#pragma once

#include "common/integer.h"

class Synchronizer
{
public:
    Synchronizer();

    void setFps(double fps);
    void beginFrame();
    void endFrame();

private:
    static u64 sleep(u32 us);

    s64 frame_delta;
    u64 frame_begin;
    u64 frame_duration;
};
