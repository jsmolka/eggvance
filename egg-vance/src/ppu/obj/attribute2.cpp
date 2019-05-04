#include "attribute2.h"

Attribute2::Attribute2(u16 data)
    : data(data)
    , tile_number(this->data)
    , priority(this->data)
    , palette_number(this->data)
{

}
