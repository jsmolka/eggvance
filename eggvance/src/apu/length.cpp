#include "length.h"

Length::Length(uint base)
    : base(base)
{

}

void Length::init()
{
    // Todo: if timer != 0?
    timer = base - length;
}

void Length::tick()
{
    if (timer && --timer == 0 && !expire)
        init();
}

bool Length::isEnabled() const
{
    return timer > 0;
}
