#pragma once

#include "square.h"

class Square2 final : public Square
{
public:
    Square2();

    void write(uint index, u8 byte);
};
