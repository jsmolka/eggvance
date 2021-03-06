#pragma once

#include "timerchannel.h"

class Timer
{
public:
    Timer();

    TimerChannel channels[4] = { 0, 1, 2, 3 };
};

inline Timer timer;
