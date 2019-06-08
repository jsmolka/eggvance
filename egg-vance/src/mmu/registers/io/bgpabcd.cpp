#include "bgpabcd.h"

Bgpabcd::Bgpabcd(u16& data)
    : Register<u16>(data)
    , fractal(data)
    , integer(data)
    , sign(data)
{

}

float Bgpabcd::value() const
{
    int s = sign ? 0xFFFFFF80 : 0;
    int i = integer | s;
    float f = static_cast<float>(fractal) / 256.0f;

    return static_cast<float>(i) + f;
}
