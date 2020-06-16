#pragma once

#include "base/register.h"

struct IrqIo
{
    XRegister<2, 0x0001> master;
    XRegister<2, 0x3FFF> enable;

    class IrqRequest : public XRegister<2, 0x3FFF>
    {
    public:
        template<uint index>
        void write(u8 byte)
        {
            data[index] &= ~(byte & byteMask<index>());
        }
    } request;
};
