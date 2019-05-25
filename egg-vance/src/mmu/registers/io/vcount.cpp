#include "vcount.h"

Vcount::Vcount(u16& data)
    : Register<u16>(data)
    , line(data)
{ 

}
