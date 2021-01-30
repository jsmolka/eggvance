#include "length.h"

Length::Length(uint base)
    : base(base)
{

}

Length& Length::operator=(uint value)
{
    length = value;

    init();

    return *this;
}

void Length::init()
{
    timer = base - length;
}

void Length::tick()
{
    if (timer && --timer == 0 && !expire)
        init();
}

uint Length::enabled() const
{
    return timer > 0;
}
