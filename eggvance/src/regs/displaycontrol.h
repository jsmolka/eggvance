#pragma once

#include "register.h"

class DisplayControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    bool hasContent() const;

    int mode;
    int frame;
    int oam_hblank;
    int mapping_1d;
    int force_blank;
    int layers;
    int win0;
    int win1;
    int winobj;
};
