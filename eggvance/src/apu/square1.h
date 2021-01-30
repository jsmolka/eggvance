#pragma once

#include "square.h"

class Square1 : public Square
{
public:
    Square1();

    void init();
    void write(uint index, u8 byte);

private:
    enum NR { k10 = 0, k11 = 2, k12 = 3, k13 = 4, k14 = 5 };
};
