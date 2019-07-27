#pragma once

struct WindowRangeH
{
    int x2;  // Right coordiante of window, plus 1 (x2 > 240 or x1 > x2 -> x2 = 240)
    int x1;  // Left coordinate of window
};
