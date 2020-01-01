#pragma once

#include <functional>

#include "common/integer.h"

struct GPR
{
    inline void reset()
    {
        value    = 0;
        on_write = std::function<void()>();
    }

    inline GPR& operator=(u32 value)
    {
        this->value = value;

        if (on_write)
            on_write();

        return *this;
    }

    inline GPR& operator=(const GPR& other)
    {
        return *this = other.value;
    }

    inline GPR& operator+=(u32 value)
    {
        return *this = this->value + value;
    }

    inline GPR& operator-=(u32 value)
    {
        return *this = this->value - value;
    }

    inline operator u32() const
    {
        return value;
    }

    u32 value;
    std::function<void()> on_write;
};
