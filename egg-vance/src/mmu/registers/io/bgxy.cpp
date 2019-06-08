#include "bgxy.h"

Bgxy::Bgxy(u32& data)
    : Register<u32>(data)
    , fractal(data)
    , integer(data)
    , sign(data)
{

}

float Bgxy::value() const
{
    int s = sign ? 0xFFF80000 : 0;
    int i = integer | s;
    float f = static_cast<float>(fractal) / 256.0f;

    return static_cast<float>(i) + f;
}
