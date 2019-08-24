#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct DMAAddress
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    u32 addr;       // Address
    u32 addr_mask;  // Address mask
};

template<unsigned index>
inline void DMAAddress::write(u8 byte)
{
    static_assert(index <= 3);

    bytes(&addr)[index] = byte;
    addr &= addr_mask;
}
