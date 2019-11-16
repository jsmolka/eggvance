#include "texture.h"

bool Texture::inBounds(int x, int y) const
{
    return this->x >= 0
        && this->y >= 0
        && this->x < x
        && this->y < y;
}
