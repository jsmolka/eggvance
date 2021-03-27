#pragma once

#include "enums.h"
#include "base/array.h"
#include "base/int.h"

class Pipeline : public array<u32, 2>
{
public:
    Access access = Access::NonSequential;
};
