#pragma once

#include "apuio.h"

class APU
{
public:
    friend class IO;

private:
    APUIO io;
};

extern APU apu;
