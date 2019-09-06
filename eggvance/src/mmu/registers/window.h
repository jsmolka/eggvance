#pragma once

#include "common/utility.h"
#include "ppu/layer.h"

struct Window
{
    void reset();

    inline void write(u8 byte);

    int bg0;  // BG0 enabled
    int bg1;  // BG1 enabled
    int bg2;  // BG2 enabled
    int bg3;  // BG3 enabled
    int obj;  // OBJ enabled
    int sfx;  // SFX enabled

    int flags;
};

void Window::write(u8 byte)
{
    bg0 = bits<0, 1>(byte);
    bg1 = bits<1, 1>(byte);
    bg2 = bits<2, 1>(byte);
    bg3 = bits<3, 1>(byte);
    obj = bits<4, 1>(byte);
    sfx = bits<5, 1>(byte);

    flags = 0;
    if (bg0) flags |= LF_BG0;
    if (bg1) flags |= LF_BG1;
    if (bg2) flags |= LF_BG2;
    if (bg3) flags |= LF_BG3;
    if (obj) flags |= LF_OBJ;
}

struct WindowInside
{
    void reset();

    Window win0;
    Window win1;
};

struct WindowOutside
{
    void reset();

    Window winout;
    Window winobj;
};
