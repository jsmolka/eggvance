#include "bldalpha.h"

#include <algorithm>

void BldAlpha::reset()
{
    *this = {};
}

void BldAlpha::update()
{
    eva = std::min(16, reg.eva);
    evb = std::min(16, reg.evb);
}
