#pragma once

#include "common/integer.h"

template<int size>
class Register
{
public:
    virtual void reset() = 0;

    virtual u8 readByte(int index) = 0;
    virtual void writeByte(int index, u8 byte) = 0;

protected:
    u8 bytes[size];
};
