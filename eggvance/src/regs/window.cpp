#include "window.h"

#include "ppu/layers.h"

void Window::reset()
{
    *this = {};
}

u8 Window::readByte()
{
    u8 byte = 0;
    byte |= bits<0, 5>(flags) << 0;
    byte |= sfx               << 5;
    return byte;
}

void Window::writeByte(u8 byte)
{
    flags = bits<0, 5>(byte) | LF_BDP;
    sfx   = bits<5, 1>(byte);
}

void WinIn::reset()
{
    win0.reset();
    win1.reset();
}

void WinOut::reset()
{
    winobj.reset();
    winout.reset();
}
