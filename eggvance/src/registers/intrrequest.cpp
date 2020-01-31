#include "intrrequest.h"

IntrRequest::IntrRequest()
{
    request = 0;
}

void IntrRequest::reset()
{
    *this = IntrRequest();
}
