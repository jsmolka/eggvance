#pragma once

#include "dmaaddress.h"
#include "dmacontrol.h"
#include "dmacount.h"

struct DMAIO
{
    DMACount count;
    DMAAddress sad;
    DMAAddress dad;
    DMAControl control;
};