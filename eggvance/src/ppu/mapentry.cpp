#include "mapentry.h"

#include "common/utility.h"

MapEntry::MapEntry(u16 data)
{
    tile   = bits< 0, 10>(data);
    flip_x = bits<10,  1>(data);
    flip_y = bits<11,  1>(data);
    bank   = bits<12,  4>(data);
}
