#pragma once

#include "register.h"

class WindowDimension : public Register<2>
{
public:
    WindowDimension(int limit);

    void reset();

    void write(int index, u8 byte);

    bool contains(int x) const;

    int min;
    int max;

private:
    int limit;
};
