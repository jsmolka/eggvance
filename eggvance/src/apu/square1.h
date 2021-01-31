#pragma once

#include "square.h"

class Square1 final : public Square
{
public:
    Square1();

    void write(uint index, u8 byte);

protected:
    void init();
};
