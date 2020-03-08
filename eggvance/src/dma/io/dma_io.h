#pragma once

#include "dmaaddress.h"
#include "dmacontrol.h"
#include "dmacount.h"

struct DmaIo
{
    DMACount count;
    DMAAddress sad;
    DMAAddress dad;
    DMAControl control;
};