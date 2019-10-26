#include "timerdata.h"

#include "common/macros.h"

void TimerData::reset()
{
    *this = {};
}

u8 TimerData::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(data)[index];
}

void TimerData::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(initial)[index] = byte;
}
