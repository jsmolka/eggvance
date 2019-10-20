#pragma once

#include "common/macros.h"
#include "common/utility.h"

class Window
{
public:
    void reset();

    u8 readByte();
    void writeByte(u8 byte);

    int flags;
    int sfx;
};

class WinIn
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    Window win0;
    Window win1;
};

class WinOut
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    Window winout;
    Window winobj;
};

#include "window.inl"
