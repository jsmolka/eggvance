#include "window.h"

void Window::reset()
{
    bg0   = 0;
    bg1   = 0;
    bg2   = 0;
    bg3   = 0;
    obj   = 0;
    sfx   = 0;
    flags = 0;
}

void WindowInside::reset()
{
    win0.reset();
    win1.reset();
}

void WindowOutside::reset()
{
    winout.reset();
    winobj.reset();
}
