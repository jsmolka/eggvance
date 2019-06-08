#include "mosaic.h"

Mosaic::Mosaic(u16& data)
    : Register<u16>(data)
    , bg_x(data)
    , bg_y(data)
    , sprite_x(data)
    , sprite_y(data)
{

}
