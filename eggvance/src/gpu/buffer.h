#pragma once

#include <array>

#include "constants.h"
#include "base/int.h"

template<typename T>
using Buffer = std::array<T, kScreen.x>;

template<typename T>
class DoubleBuffer
{
public:
    T* data()
    {
        return buffer[page].data();
    }

    void flip()
    {
        page ^= 1;
    }

    void fill(const T& value)
    {
        buffer[0].fill(value);
        buffer[1].fill(value);
    }

    T& operator[](uint index)
    {
        return buffer[page][index];
    }

    const T& operator[](uint index) const
    {
        return buffer[page][index];
    }

private:
    uint page = 0;
    Buffer<T> buffer[2];
};
