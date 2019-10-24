#include "bldy.h"

#include <algorithm>

void BldY::reset()
{
    *this = {};
}

void BldY::update()
{
    evy = std::min(16, reg.evy);
}
