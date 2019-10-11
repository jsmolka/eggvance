#pragma once

#include "common/integer.h"

struct Window
{
    void reset();

    u8 readByte();
    void writeByte(u8 byte);

    int bg0;  // BG0 enabled
    int bg1;  // BG1 enabled
    int bg2;  // BG2 enabled
    int bg3;  // BG3 enabled
    int obj;  // OBJ enabled
    int sfx;  // SFX enabled

    int flags;
};

struct WindowInside
{
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    Window win0;
    Window win1;
};

struct WindowOutside
{
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    Window winout;
    Window winobj;
};
