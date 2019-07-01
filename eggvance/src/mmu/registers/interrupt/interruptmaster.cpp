#include "interruptmaster.h"

InterruptMaster::InterruptMaster(u32& data)
    : Register<u32>(data)
    , enabled(data)
{

}
