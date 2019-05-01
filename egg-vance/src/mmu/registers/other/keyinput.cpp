#include "keyinput.h"

Keyinput::Keyinput(u16& data)
    : Register(data)
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
{

}
