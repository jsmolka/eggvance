#include "bgxy.h"

Bgxy::Bgxy(u32& data)
    : Register<u32>(data)
    , fractal(data)
    , integer(data)
    , sign(data)
{

}
