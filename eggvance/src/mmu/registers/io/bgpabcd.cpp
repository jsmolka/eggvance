#include "bgpabcd.h"

Bgpabcd::Bgpabcd(u16& data)
    : Register<u16>(data)
    , fractal(data)
    , integer(data)
    , sign(data)
{

}
