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
    if (expire && timer)
        timer--;
}

uint Length::enabled() const
{
    return timer > 0;
}
