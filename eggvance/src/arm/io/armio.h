#pragma once

#include "haltcontrol.h"
#include "waitcontrol.h"

struct ARMIO
{
    HaltControl haltcnt;
    WaitControl waitcnt;
};
