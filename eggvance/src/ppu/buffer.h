#pragma once

#include <shell/array.h>
#include <shell/ranges.h>

#include "constants.h"
#include "base/int.h"

template<typename T>
using ScanlineBuffer = shell::array<T, kScreen.x>;

template<typename T>
class ScanlineDoubleBuffer
{
public:
    using iterator               = typename shell::array<T, kScreen.x>::iterator;
    using const_iterator         = typename shell::array<T, kScreen.x>::const_iterator;
    using reverse_iterator       = typename shell::array<T, kScreen.x>::reverse_iterator;
    using const_reverse_iterator = typename shell::array<T, kScreen.x>::const_reverse_iterator;

    T* data()
    {
        return pages[page].data();
    }

    const T* data() const
    {
        return pages[page].data();
    }

    void flip()
    {
        page ^= 0x1;
    }

    T& operator[](uint index)
    {
        return pages[page][index];
    }

    const T& operator[](uint index) const
    {
        return pages[page][index];
    }

    SHELL_FORWARD_ITERATORS(pages[page].begin(), pages[page].end())
    SHELL_REVERSE_ITERATORS(pages[page].end(), pages[page].begin())

private:
    shell::uint page = 0;
    shell::array<T, 2, kScreen.x> pages = {};
};
