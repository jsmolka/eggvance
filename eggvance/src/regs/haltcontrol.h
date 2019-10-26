#pragma once

#include "register.h"

class HaltControl : public Register<1>
{
public:
    operator bool() const;
    HaltControl& operator=(bool value);

    void reset();

    void writeByte(int index, u8 byte);

private:
    bool halt;
};
