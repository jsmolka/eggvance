#pragma once

#include <array>

#include "enums.h"
#include "base/int.h"

class Pipeline
{
public:
    u32  operator[](uint index) const;
    u32& operator[](uint index);

    Access access = Access::NonSequential;

private:
    std::array<u32, 2> data = {};
};
