#include "length.h"

void Length::init()
{
    if (value == 0)
        value = initial;
}

void Length::tick()
{
    if (value > 0)
        value--;
}
