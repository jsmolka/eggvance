#include "winv.h"

Winv::Winv(u16& data)
    : Register<u16>(data)
    , y1(data)
    , y2(data)
{

}
