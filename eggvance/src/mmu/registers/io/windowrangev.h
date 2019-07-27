#pragma once

struct WindowRangeV
{
    int y2;  // Bottom coordiante of window, plus 1 (y2 > 160 or y1 > y2 -> y2 = 240)
    int y1;  // Top coordinate of window
};
