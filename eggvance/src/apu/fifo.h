#pragma once

#include <shell/ringbuffer.h>

#include "base/int.h"

class Fifo : public shell::RingBuffer<s8, 32>
{
public:
    bool refillable() const
    {
        return size() <= 16;
    }

    s8 sample = 0;
};
