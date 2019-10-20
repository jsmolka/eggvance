#include "dispcnt.h"

void DispCnt::reset()
{
    *this = {};
}

void DispCnt::update()
{
    switch (mode)
    {
    case 0: has_content = bg[0] || bg[1] || bg[2] || bg[3]; break;
    case 1: has_content = bg[0] || bg[1] || bg[2]; break;
    case 2: has_content = bg[2] || bg[3]; break;
    case 3: has_content = bg[2]; break;
    case 4: has_content = bg[2]; break;
    case 5: has_content = bg[2]; break;

    default:
        has_content = false;
        break;
    }
    has_content |= obj;
}
