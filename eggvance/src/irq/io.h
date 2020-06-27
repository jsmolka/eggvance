#pragma once

#include "base/register.h"

struct IrqIo
{
    Register<4, 0x0001> master;
    Register<2, 0x3FFF> enable;

    class Request : public Register<2, 0x3FFF>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            static_assert(Index < kSize);

            data[Index] &= ~(byte & (kMask >> (CHAR_BIT * Index)));
        }
    } request;
};
