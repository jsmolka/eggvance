#include "ime.h"

Ime::Ime(u32& data)
    : Register<u32>(data)
    , enabled(data)
{

}
