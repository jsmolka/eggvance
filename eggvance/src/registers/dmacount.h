#pragma once

#include "register.h"
#include "common/bits.h"

class DMACount : public Register<DMACount, 2>
{
public:
    DMACount(uint limit)
        : limit(limit) {}

    inline void reset()
    {
        *this = DMACount(limit);
    }

    template<uint index>
    inline u8 read() const = delete;

    inline uint count()
    {
        if (cast<u16>() == 0)
            return limit;
        else
            return cast<u16>();
    }

private:
    uint limit;
};
