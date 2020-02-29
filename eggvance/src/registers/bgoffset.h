#pragma once

#include "register.h"

class BGOffset : public RegisterRW<2>
{
public:
    inline operator u16()
    {
        return cast<u16>();
    }

    template<uint index>
    inline u8 read() const = delete;
};
