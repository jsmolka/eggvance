#pragma once

#include "square.h"

class Square2 : public Square
{
public:
    Square2();

    void write(uint index, u8 byte);

private:
    enum NR { k21 = 0, k22 = 1, k23 = 4, k24 = 5 };
};
