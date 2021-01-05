#pragma once

#include <shell/ringbuffer.h>

#include "base/int.h"

class Fifo : public shell::RingBuffer<u8, 32>
{
public:
    bool refillable() const
    {
        return size() <= 16;
    }

    s16 sample = 0;
};
