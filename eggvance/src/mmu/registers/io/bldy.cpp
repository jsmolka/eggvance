#include "bldy.h"

Bldy::Bldy(u16& data)
    : Register<u16>(data)
    , evy(data)
{

}
