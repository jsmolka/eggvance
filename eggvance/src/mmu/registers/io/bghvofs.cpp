#include "bghvofs.h"

Bghvofs::Bghvofs(u16& data)
    : Register<u16>(data)
    , offset(data)
{

}
