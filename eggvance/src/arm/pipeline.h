#pragma once

#include <shell/array.h>

#include "enums.h"
#include "base/int.h"

class Pipeline : public shell::array<u32, 2>
{
public:
    Access access = Access::NonSequential;
};
