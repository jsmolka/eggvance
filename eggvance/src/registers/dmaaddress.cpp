#include "dmaaddress.h"

#include "common/macros.h"

void DMAAddress::reset()
{
    *this = {};
}

void DMAAddress::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    bcast(addr)[index] = byte;
}
