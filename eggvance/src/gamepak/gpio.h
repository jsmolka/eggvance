#pragma once

#include "base/int.h"

class Gpio
{
public:
    enum class Type
    {
        None,
        Rtc
    };

    Gpio();
    explicit Gpio(Type type);
    virtual ~Gpio() = default;

    virtual void reset();

    virtual u16 read(u32 addr);
    virtual void write(u32 addr, u16 half);

    const Type type;
};
