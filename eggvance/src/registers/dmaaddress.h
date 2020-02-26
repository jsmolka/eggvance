#pragma once

#include "register.h"

class DMAAddress : public Register<DMAAddress, 4>
{
public:
    inline operator u32()
    {
        return *reinterpret_cast<u32*>(data);
    }

    template<uint index>
    inline u8 read() const = delete;
};
