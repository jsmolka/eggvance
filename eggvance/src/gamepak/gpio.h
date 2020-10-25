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

    const Type type;
};
