#pragma once

#include "register.h"

class Window : Register<1>
{
public:
    void reset();

    u8 readByte();
    void writeByte(u8 byte);

    int flags;
    int sfx;
};

class WindowIn : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    Window win0;
    Window win1;
};

class WindowOut : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    Window winout;
    Window winobj;
};
