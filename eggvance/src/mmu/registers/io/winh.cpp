#include "winh.h"

Winh::Winh(u16& data)
    : Register<u16>(data)
    , x1(data)
    , x2(data)
{

}
