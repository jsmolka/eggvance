#include "mosaic.h"

void Mosaic::Stretch::reset()
{
    *this = {};
}

void Mosaic::reset()
{
    bgs.reset();
    obj.reset();
}
