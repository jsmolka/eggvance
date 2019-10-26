#pragma once

#include "register.h"

class DMACount : public Register<2>
{
public:
    DMACount(int limit);

    void reset();

    void write(int index, u8 byte);

    int count() const;

private:
    int limit;
};
