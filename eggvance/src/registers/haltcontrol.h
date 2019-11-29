#pragma once

#include "register.h"

class HaltControl : public Register<1>
{
public:
    operator bool() const;
    HaltControl& operator=(bool value);

    void reset();

    void write(int index, u8 byte);

    bool halt;
};
