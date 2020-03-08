#pragma once

#include "intrenable.h"
#include "intrmaster.h"
#include "intrrequest.h"

struct IntrIO
{
    IntrMaster master;
    IntrEnable enable;
    IntrRequest request;
};
