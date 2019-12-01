#pragma once

#ifdef TRANSPARENT
#  undef TRANSPARENT
#endif

constexpr int SCREEN_W = 240;
constexpr int SCREEN_H = 160;

constexpr int COLOR_MASK  = 0x7FFF;
constexpr int TRANSPARENT = 0x8000;

constexpr double REFRESH_RATE = 59.737;
