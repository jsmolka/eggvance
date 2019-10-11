#pragma once

#include "common/integer.h"

struct BackgroundReference
{
    void reset();

    void writeByte(int index, u8 byte);

    int reference;  // Reference point
    int internal;   // Internal register (reference copied during V-Blank or write)
};
