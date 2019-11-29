#include "dmacount.h"

#include "common/macros.h"

DMACount::DMACount(int limit)
{
    this->limit = limit;
}

void DMACount::reset()
{
    *this = { limit };
}

void DMACount::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    data[index] = byte;
}

int DMACount::count() const
{
    int count = data[0] | data[1] << 8;
    if (count == 0)
        return limit;
    else
        return count;
}
