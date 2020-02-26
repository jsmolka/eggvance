#pragma once

#include "register.h"

class KeyInput : public Register<KeyInput, 2>
{
public:
    KeyInput()
    {
        cast<u16>() = 0x03FF;
    }

    inline KeyInput& operator=(u16 value)
    {
        cast<u16>() = value;

        return *this;
    }

    inline operator u16()
    {
        return cast<u16>();
    }

    template<uint index>
    inline void write(u8 byte) = delete;
};
