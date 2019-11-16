#pragma once

struct Dimensions
{
    constexpr Dimensions(int w, int h)
        : w(w), h(h) { }

    int w;
    int h;
};
