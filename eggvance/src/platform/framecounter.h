#pragma once

#include "common/integer.h"

class FrameCounter
{
public:
    FrameCounter();

    FrameCounter& operator++();

    bool fps(double& value);

private:
    u64 start;
    int count;
};
