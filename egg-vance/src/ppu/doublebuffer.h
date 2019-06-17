#pragma once

#include <array>

#include "common/integer.h"

class DoubleBuffer
{
public:
    void flip();
    void copyPage();

    u16  operator[](int index) const;
    u16& operator[](int index);

private:
    int page;

    std::array<std::array<u16, 240>, 2> buffer;
};
