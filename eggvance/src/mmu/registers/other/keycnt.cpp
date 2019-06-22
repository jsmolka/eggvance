#include "keycnt.h"

Keycnt::Keycnt(u16& data)
    : Register<u16>(data)
    , a(data)
    , b(data)
    , select(data)
    , start(data)
    , right(data)
    , left(data)
    , up(data)
    , down(data)
    , r(data)
    , l(data)
    , irq(data)
    , logic(data)
{

}
