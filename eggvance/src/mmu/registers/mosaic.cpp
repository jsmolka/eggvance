#include "mosaic.h"

void Mosaic::Stretch::reset()
{
    x = 0;
    y = 0;
}

void Mosaic::reset()
{
    bg.reset();
    obj.reset();
}
