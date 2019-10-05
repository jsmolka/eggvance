#pragma once

#include "registerbank.h"

class Registers : public RegisterBank
{
public:
    Registers();

    void reset();

    u32  operator[](int index) const;
    u32& operator[](int index);
};
