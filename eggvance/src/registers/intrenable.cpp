#include "intrenable.h"

IntrEnable::IntrEnable()
{
    enable = 0;
}

void IntrEnable::reset()
{
    *this = IntrEnable();
}
