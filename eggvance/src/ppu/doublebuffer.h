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
            pages[page ^ 1].begin(),
            pages[page ^ 1].end(),
            pages[page].begin()
        );
    }

    inline Buffer<T>& buffer() const
    {
        return buffer[page];
    }

    inline T operator[](int index) const
    {
        return pages[page][index];
    }

    inline T& operator[](int index)
    {
        return pages[page][index];
    }

private:
    int page = 0;
    Buffer<T> pages[2];
};
