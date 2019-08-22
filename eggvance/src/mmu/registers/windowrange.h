#pragma once

#include "common/integer.h"

class WindowRange
{
public:
    void write(int index, u8 byte);

    int min;  // Min coordiante of window axis
    int max;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit) 
};
