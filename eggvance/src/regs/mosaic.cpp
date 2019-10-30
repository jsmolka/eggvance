#include "mosaic.h"

#include "common/macros.h"
#include "common/utility.h"

void Mosaic::Stretch::reset()
{
    x = 1;
    y = 1;
}

int Mosaic::Stretch::sourceX(int x) const
{
    return this->x * (x / this->x);
}

int Mosaic::Stretch::sourceY(int y) const
{
    return this->y * (y / this->y);
}

void Mosaic::reset()
{
    bgs.reset();
    obj.reset();
}

void Mosaic::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        bgs.x = bits<0, 4>(byte) + 1;
        bgs.y = bits<4, 4>(byte) + 1;
    }
    else
    {
        obj.x = bits<0, 4>(byte) + 1;
        obj.y = bits<4, 4>(byte) + 1;
    }
}
