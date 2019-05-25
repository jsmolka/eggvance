#include "bghofs.h"

Bghofs::Bghofs(u16& data) 
    : Register<u16>(data)
    , offset(data)
{

}
