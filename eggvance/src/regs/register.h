#pragma once

#include "common/integer.h"

template<int size>
class Register
{
protected:
    u8 bytes[size];
};
