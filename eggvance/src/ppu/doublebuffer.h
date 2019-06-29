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

    inline T operator[](int index) const
    {
        return buffer[page][index];
    }

    inline T& operator[](int index)
    {
        return buffer[page][index];
    }

private:
    int page = 0;
    Buffer<T> buffer[2];
};
