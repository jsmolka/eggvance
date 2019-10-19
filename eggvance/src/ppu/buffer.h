#pragma once

#include <array>

template<typename T>
using Buffer = std::array<T, 240>;

template<typename T>
class DoubleBuffer
{
public:
    void flip()
    {
        page ^= 1;
    }

    T* data()
    {
        return buffer[page].data();
    }

    void fill(const T& value)
    {
        buffer[page].fill(value);
    }

    T operator[](std::size_t index) const
    {
        return buffer[page][index];
    }

    T& operator[](std::size_t index)
    {
        return buffer[page][index];
    }

private:
    int page = 0;
    Buffer<T> buffer[2];
};
