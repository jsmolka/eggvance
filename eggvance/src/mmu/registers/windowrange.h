#pragma once

struct WindowRange
{
    int min;  // Min coordiante of window axis
    int max;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit) 
};
