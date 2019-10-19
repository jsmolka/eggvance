#include "window.h"

#include "common/macros.h"
#include "common/utility.h"
#include "ppu/layers.h"

void Window::reset()
{
    bg0   = 0;
    bg1   = 0;
    bg2   = 0;
    bg3   = 0;
    obj   = 0;
    sfx   = 0;
    flags = LF_BDP;
}

u8 Window::readByte()
{
    u8 byte = 0;
    byte |= bg0 << 0;
    byte |= bg1 << 1;
    byte |= bg2 << 2;
    byte |= bg3 << 3;
    byte |= obj << 4;
    byte |= sfx << 5;
    return byte;
}

void Window::writeByte(u8 byte)
{
    bg0   = bits<0, 1>(byte);
    bg1   = bits<1, 1>(byte);
    bg2   = bits<2, 1>(byte);
    bg3   = bits<3, 1>(byte);
    obj   = bits<4, 1>(byte);
    sfx   = bits<5, 1>(byte);
    flags = (byte & 0x1F) | LF_BDP;
}

void WindowInside::reset()
{
    win0.reset();
    win1.reset();
}

u8 WindowInside::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: return win0.readByte();
    case 1: return win1.readByte();

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void WindowInside::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: win0.writeByte(byte); break;
    case 1: win1.writeByte(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void WindowOutside::reset()
{
    winobj.reset();
    winout.reset();
}

u8 WindowOutside::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: return winobj.readByte();
    case 1: return winout.readByte();

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void WindowOutside::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: winobj.writeByte(byte); break;
    case 1: winout.writeByte(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
