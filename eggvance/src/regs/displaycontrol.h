#pragma once

#include "register.h"

enum ObjectMapping
{
    MAP_2D = 0,
    MAP_1D = 1
};

class DisplayControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    bool isActive() const;
    bool isBitmap() const;


    int mode;
    int frame;
    int oam_hblank;
    int obj_mapping;
    int force_blank;
    int layers;
    int win0;
    int win1;
    int winobj;

    u32 base_frame;

private:
    void update();
};
