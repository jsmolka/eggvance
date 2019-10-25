#include "irqrequest.h"

#include "common/macros.h"
#include "common/utility.h"

IRQRequest::operator int() const
{
    return value;
}

IRQRequest& IRQRequest::operator|=(int value)
{
    this->value |= value;
    return *this;
}

void IRQRequest::reset()
{
    *this = {};
}

u8 IRQRequest::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return bits<0, 8>(value);
    else
        return bits<8, 8>(value);
}

void IRQRequest::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    byteArray(value)[index] &= ~byte;
}
