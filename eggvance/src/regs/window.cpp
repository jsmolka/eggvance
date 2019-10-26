#include "window.h"

#include "common/macros.h"
#include "common/utility.h"
#include "ppu/layers.h"

void Window::reset()
{
    *this = {};
}

u8 Window::readByte(int index)
{
    EGG_ASSERT(index == 0, "Invalid index");
    return bytes[0];
}

void Window::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0, "Invalid index");
    bytes[0] = byte;
    flags = bits<0, 5>(byte) | LF_BDP;
    sfx   = bits<5, 1>(byte);
}

void WindowIn::reset()
{
    win0.reset();
    win1.reset();
}

u8 WindowIn::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return win0.readByte(0);
    else
        return win1.readByte(0);
}

void WindowIn::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return win0.writeByte(0, byte);
    else
        return win1.writeByte(0, byte);
}

void WindowOut::reset()
{
    winobj.reset();
    winout.reset();
}

u8 WindowOut::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return winout.readByte(0);
    else
        return winobj.readByte(0);
}

void WindowOut::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return winout.writeByte(0, byte);
    else
        return winobj.writeByte(0, byte);
}
