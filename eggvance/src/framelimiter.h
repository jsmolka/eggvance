#pragma once

#include "common/integer.h"

class FrameLimiter
{
public:
    FrameLimiter();

    void setFPS(double fps);

    void frameBegin();
    void frameSleep();

private:
    void reset();

    u64 begin;
    u64 frame;
    s64 delta;
};
