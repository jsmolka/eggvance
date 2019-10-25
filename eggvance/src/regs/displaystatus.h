#pragma once

#include "register.h"

class DisplayStatus : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int vblank;
    int hblank;
    int vmatch;
    int vblank_irq;
    int hblank_irq;
    int vmatch_irq;
    int vcompare;
};
