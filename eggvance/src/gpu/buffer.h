#pragma once

#include <array>

#include "gpu/constants.h"

template<typename T>
using Buffer = std::array<T, kScreen.x>;

template<typename T>
class DoubleBuffer
{
public:
    constexpr DoubleBuffer()
        : page(0)
    {

    }

    constexpr T* data()
    {
        return buffer[page].data();
    }

    constexpr void flip()
    {
        page ^= 1;
    }

    constexpr void fill(const T& value)
    {
        buffer[page].fill(value);
    }

    constexpr T operator[](std::size_t index) const
    {
        return buffer[page][index];
    }

    constexpr T& operator[](std::size_t index)
    {
        return buffer[page][index];
    }

private:
    int page;
    Buffer<T> buffer[2];
};
