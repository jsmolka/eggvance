#pragma once

#include "common/delegate.h"
#include "common/integer.h"

class ARM;

class GPR
{
public:
    inline void onWrite(const Delegate<ARM>& callback)
    {
        this->callback = callback;
    }

    inline GPR& operator=(u32 value)
    {
        this->value = value;

        if (callback)
            callback();

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

    u32 value = 0;

private:
    Delegate<ARM> callback;
};
