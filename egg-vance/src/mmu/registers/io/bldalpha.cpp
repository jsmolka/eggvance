#include "bldalpha.h"

Bldalpha::Bldalpha(u16& data)
    : Register<u16>(data)
    , eva(data)
    , evb(data)
{

}
