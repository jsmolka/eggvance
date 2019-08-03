#pragma once

#include "buffer.h"

template<typename T>
class DoubleBuffer
{
public:
    inline void flip()
    {
        page ^= 1;
    }

    inline void fill(const T& value)
    {
        buffer[page].fill(value);
    }

    inline T operator[](std::size_t index) const
    {
        return buffer[page][index];
    }

    inline T& operator[](std::size_t index)
    {
        return buffer[page][index];
    }

private:
    int page = 0;
    Buffer<T> buffer[2];
};
