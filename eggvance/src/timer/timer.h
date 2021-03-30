#pragma once

#include <shell/array.h>

#include "timerchannel.h"

class Timer
{
public:
    Timer();

    shell::array<TimerChannel, 4> channels = { 0, 1, 2, 3 };
};

inline Timer timer;
