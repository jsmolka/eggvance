#include "bgxy.h"

#include "common/utility.h"

Bgxy::Bgxy(u32& data)
    : Register<u32>(data)
    , fractal(data)
    , integer(data)
    , sign(data)
{

}

void Bgxy::moveToInternal()
{
    internal = signExtend<int, 28>(static_cast<int>(*this));
}
