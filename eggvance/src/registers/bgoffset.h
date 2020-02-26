#pragma once

#include "register.h"

class BGOffset : public Register<BGOffset, 2>
{
public:
    inline operator u16()
    {
        return *reinterpret_cast<u16*>(data);
    }

    template<uint index>
    inline u8 read() const = delete;
};
