#include "dmaaddress.h"

#include "common/macros.h"
#include "common/utility.h"

void DMAAddress::reset()
{
    addr = 0;
}

u8 DMAAddress::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    return 0;
}

void DMAAddress::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    byteArray(addr)[index] = byte;
    addr &= addr_mask;
}
