#pragma once

#include "register.h"

class HaltControl : public RegisterW<1>
{
public:
    template<uint index>
    inline void write(u8 byte);
};
