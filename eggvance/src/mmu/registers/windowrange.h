#pragma once

template<int limit>
struct WindowRange
{
    int min;  // Min coordiante of window axis
    int max;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit) 

    int max_adj;
    inline void adjust()
    {
        if (max > limit || max < min)
            max_adj = limit;
        else
            max_adj = max;
    }
};
