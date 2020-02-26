#pragma once

#include "register.h"

class HaltControl : public Register<HaltControl, 1>
{
public:
    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte);
};
