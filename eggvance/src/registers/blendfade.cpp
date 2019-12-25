#include "blendfade.h"

#include <algorithm>

#include "common/bits.h"
#include "common/macros.h"

void BlendFade::reset()
{
    *this = {};
}

void BlendFade::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        evy = std::min(16, static_cast<int>(bits<0, 5>(byte)));
}
