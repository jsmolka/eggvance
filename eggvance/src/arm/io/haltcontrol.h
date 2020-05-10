#pragma once

#include "base/register.h"

class HaltControl : public RegisterW<1>
{
public:
    template<uint index>
    void write(u8 byte);
};
