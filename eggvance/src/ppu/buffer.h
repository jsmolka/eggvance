#pragma once

#include <array>

#include "common/constants.h"

template<typename T>
using Buffer = std::array<T, SCREEN_W>;

template<typename T>
class DoubleBuffer
{
public:
    inline void flip()
    {
        page ^= 1;
    }

    inline T* data()
    {
        return buffer[page].data();
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
