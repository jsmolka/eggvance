#pragma once

#include "register.h"

class Mosaic : public Register<4>
{
public:
    struct Stretch
    {
        void reset();

        int mosaicX(int x) const;
        int mosaicY(int y) const;

        int x;
        int y;
    };

    void reset();

    void write(int index, u8 byte);

    Stretch bgs;
    Stretch obj;
};
