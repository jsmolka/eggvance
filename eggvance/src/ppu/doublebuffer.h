#pragma once

#include <algorithm>

#include "buffer.h"

template<typename T>
class DoubleBuffer
{
public:
    inline void flip()
    {
        page ^= 1;
    }

    inline void copyPage()
    {
        std::copy(
            buffer[page ^ 1].begin(),
            buffer[page ^ 1].end(),
            buffer[page].begin()
        );
    }

    inline T* data() const
    {
        return buffer[page].data();
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
