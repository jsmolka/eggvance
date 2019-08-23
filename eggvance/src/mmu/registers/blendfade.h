#pragma once

#include"common/utility.h"

struct BlendFade
{
    inline void write(u8 byte);

    int evy;  // EVY coefficient for brightness (0..16)
};

inline void BlendFade::write(u8 byte)
{
    // Todo: 16 / 17???
    evy = std::min(bits<0, 5>(byte), 16);
}
