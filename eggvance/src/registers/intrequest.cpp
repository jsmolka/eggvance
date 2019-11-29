#include "intrequest.h"

#include "common/macros.h"
#include "common/utility.h"

IntRequest::operator int() const
{
    return request;
}

IntRequest& IntRequest::operator|=(int value)
{
    this->request |= value;

    return *this;
}

void IntRequest::reset()
{
    *this = {};
}

u8 IntRequest::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(request)[index];
}

void IntRequest::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    
    bcast(request)[index] &= ~byte;
}
