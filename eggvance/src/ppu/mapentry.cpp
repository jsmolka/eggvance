#include "mapentry.h"

#include "base/bit.h"

MapEntry::MapEntry(u16 data)
{
    tile   = bit::seq< 0, 10>(data);
    flip.x = bit::seq<10,  1>(data) * 0x7;
    flip.y = bit::seq<11,  1>(data) * 0x7;
    bank   = bit::seq<12,  4>(data);
}
