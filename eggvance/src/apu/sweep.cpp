#include "sweep.h"

void Sweep::tick()
{

}

void Sweep::write(u8 byte)
{
    shift     = bit::seq<0, 3>(byte);
    direction = bit::seq<3, 1>(byte);
    time      = bit::seq<4, 2>(byte);
}
