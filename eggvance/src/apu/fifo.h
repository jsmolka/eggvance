#pragma once

#include <shell/ringbuffer.h>

#include "base/int.h"

class Fifo : public shell::RingBuffer<s8, 32>
{
public:
    void tick();

    s16  sample    = 0;
    uint volume    = 0;
    uint enabled_r = 0;
    uint enabled_l = 0;
    uint timer     = 0;
};
