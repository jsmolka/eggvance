#pragma once

#include "register.h"

class DMAControl : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int sad_delta;
    int dad_delta;
    int repeat;
    int word;
    int timing;
    int irq;
    int enabled;

    bool update;
};
