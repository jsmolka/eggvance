#include "bgvofs.h"

Bgvofs::Bgvofs(u16& data)
    : Register<u16>(data)
    , offset(data)
{

}
