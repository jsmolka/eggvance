#include "window.h"

#include "common/macros.h"
#include "common/utility.h"
#include "ppu/layers.h"

void Window::reset()
{
    *this = {};
}

u8 Window::readByte()
{
    return bytes[0];
}

void Window::writeByte(u8 byte)
{
    flags = bits<0, 5>(byte) | LF_BDP;
    sfx   = bits<5, 1>(byte);

    bytes[0] = byte;
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
        return win0.readByte();
    else
        return win1.readByte();
}

void WindowIn::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return win0.writeByte(byte);
    else
        return win1.writeByte(byte);
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
        return winout.readByte();
    else
        return winobj.readByte();
}

void WindowOut::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return winout.writeByte(byte);
    else
        return winobj.writeByte(byte);
}
