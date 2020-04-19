#include "mapentry.h"

#include "common/bits.h"

MapEntry::MapEntry(u16 data)
{
    tile   = bits::seq< 0, 10>(data);
    flip_x = bits::seq<10,  1>(data);
    flip_y = bits::seq<11,  1>(data);
    bank   = bits::seq<12,  4>(data);
}
