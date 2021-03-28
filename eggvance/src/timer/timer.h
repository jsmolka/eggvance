#pragma once

#include "timerchannel.h"
#include "base/array.h"

class Timer
{
public:
    Timer();

    array<TimerChannel, 4> channels = { 0, 1, 2, 3 };
};

inline Timer timer;
