#include "sprite.h"

void Sprite::setPixel(int x, int y, int value, bool flip_x, bool flip_y)
{
    if (flip_x) x = width  - 1 - x;
    if (flip_y) y = height - 1 - y;

    data[y][x] = value;
}
