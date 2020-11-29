#pragma once

#include "base/int.h"

class Pipeline
{
public:
    u32& operator[](uint index)
    {
        return data[index];
    }

    u32  operator[](uint index) const
    {
        return data[index];
    }

private:
    u32 data[2] = {};
};
