#pragma once

#include "common/integer.h"

class FrameLimiter
{
public:
    FrameLimiter();

    void setFPS(double fps);

    void begin();
    void end();

private:
    void reset();

    u64 frame_beg;
    u64 frame_end;
    u64 frame_time;
    s64 frame_delta;
};
