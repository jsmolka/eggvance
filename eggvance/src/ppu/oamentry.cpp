#include "oamentry.h"

OAMEntry::OAMEntry(int attr0, int attr1, int attr2)
    : y(attr0)
    , affine(attr0)
    , double_size(attr0)
    , disabled(attr0)
    , gfx_mode(attr0)
    , mosaic(attr0)
    , color_mode(attr0)
    , shape(attr0)
    , x(attr1)
    , paramter(attr1)
    , flip_h(attr1)
    , flip_v(attr1)
    , size(attr1)
    , tile(attr2)
    , priority(attr2)
    , palette_bank(attr2)
{

}

int OAMEntry::width() const
{
    return dimensions[shape][size][0];
}

int OAMEntry::height() const
{
    return dimensions[shape][size][1];
}
