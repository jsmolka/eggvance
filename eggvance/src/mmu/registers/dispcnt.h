#pragma once

#include "common/macros.h"
#include "common/utility.h"

class DispCnt
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int mode;
    int frame;
    int oam_hblank;
    int mapping_1d;
    int force_blank;
    int bg[4];
    int obj;
    int win0;
    int win1;
    int winobj;

    bool has_content;

private:
    void update();
};

#include "dispcnt.inl"
