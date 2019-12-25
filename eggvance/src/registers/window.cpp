#include "window.h"

#include "common/bits.h"
#include "common/macros.h"
#include "ppu/layer.h"

void Window::reset()
{
    *this = {};
}

u8 Window::read()
{
    return data[0];
}

void Window::write(u8 byte)
{
    flags   = bits<0, 5>(byte) | LF_BDP;
    effects = bits<5, 1>(byte);

    data[0] = byte;
}

void WindowIn::reset()
{
    win0.reset();
    win1.reset();
}

u8 WindowIn::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return win0.read();
    else
        return win1.read();
}

void WindowIn::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return win0.write(byte);
    else
        return win1.write(byte);
}

void WindowOut::reset()
{
    winobj.reset();
    winout.reset();
}

u8 WindowOut::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return winout.read();
    else
        return winobj.read();
}

void WindowOut::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return winout.write(byte);
    else
        return winobj.write(byte);
}
