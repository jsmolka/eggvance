#pragma once

#include "base/integer.h"

class Gpio
{
public:
    enum class Type
    {
        None,
        Rtc
    };

    Gpio();
    Gpio(Type type);

    virtual u16 read(u32 addr);
    virtual void write(u32 addr, u16 half);

    const Type type;
};
