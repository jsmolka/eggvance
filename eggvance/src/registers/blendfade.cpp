#include "blendfade.h"

#include <algorithm>

#include "common/macros.h"
#include "common/utility.h"

void BlendFade::reset()
{
    *this = {};
}

void BlendFade::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        evy = std::min(16, bits<0, 5>(byte));
}
