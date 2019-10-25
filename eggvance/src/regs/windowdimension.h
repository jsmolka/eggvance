#pragma once

#include "register.h"

template<int limit>
class WindowDimension : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    bool contains(int x) const;

    int min;
    int max;
};

#include "windowdimension.inl"
