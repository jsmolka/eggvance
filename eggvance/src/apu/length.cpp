#include "length.h"

Length::Length(uint base)
    : base(base)
{

}

void Length::init()
{
    if (value == 0)
        value = base - length;
}

void Length::tick()
{
    if (value && --value == 0 && !expire)
        value = base - length;
}

bool Length::isEnabled() const
{
    return value > 0;
}
