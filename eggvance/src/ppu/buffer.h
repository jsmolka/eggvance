#pragma once

#include <array>

#include "constants.h"
#include "base/int.h"

template<typename T>
using ScanlineBuffer = std::array<T, kScreen.x>;

template<typename T>
class ScanlineDoubleBuffer
{
public:
    T* data()
    {
        return buffers[page].data();
    }

    const T* data() const
    {
        return buffers[page].data();
    }

    void flip()
    {
        page ^= 0x1;
    }

    void fill(const T& value)
    {
        buffers[0].fill(value);
        buffers[1].fill(value);
    }

    T& operator[](uint index)
    {
        return buffers[page][index];
    }

    const T& operator[](uint index) const
    {
        return buffers[page][index];
    }

private:
    uint page = 0;
    ScanlineBuffer<T> buffers[2];
};
