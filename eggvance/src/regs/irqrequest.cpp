#include "irqrequest.h"

#include "common/macros.h"
#include "common/utility.h"

IRQRequest::operator int() const
{
    return request;
}

IRQRequest& IRQRequest::operator|=(int value)
{
    this->request |= value;

    return *this;
}

void IRQRequest::reset()
{
    *this = {};
}

u8 IRQRequest::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(request)[index];
}

void IRQRequest::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    
    bcast(request)[index] &= ~byte;
}
