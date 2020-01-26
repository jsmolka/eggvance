#pragma once

#include "common/delegate.h"
#include "common/integer.h"

class ARM;

class GPR
{
public:
    void onWrite(const Delegate<ARM>& callback)
    {
        this->callback = callback;
    }

    GPR& operator=(u32 value)
    {
        this->value = value;

        if (callback)
            callback();

        return *this;
    }

    GPR& operator=(const GPR& other)
    {
        return *this = other.value;
    }

    GPR& operator+=(u32 value)
    {
        return *this = this->value + value;
    }

    GPR& operator-=(u32 value)
    {
        return *this = this->value - value;
    }

    operator u32() const
    {
        return value;
    }

    u32 value = 0;

private:
    Delegate<ARM> callback;
};
