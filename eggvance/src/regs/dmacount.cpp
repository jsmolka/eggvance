#include "dmacount.h"

#include "common/macros.h"

void DMACount::reset()
{

}

void DMACount::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    data[index] = byte;
}

int DMACount::count()
{
    int count = data[0] | data[1] << 8;
    if (count == 0)
        return limit;
    else
        return count;
}
