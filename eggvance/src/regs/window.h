#pragma once

#include "register.h"

class Window : Register<1>
{
public:
    void reset();

    u8 read();
    void write(u8 byte);

    int flags;
    int effects;
};

class WindowIn : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    Window win0;
    Window win1;
};

class WindowOut : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    Window winout;
    Window winobj;
};
