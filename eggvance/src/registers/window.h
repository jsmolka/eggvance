#pragma once

#include "register.h"
#include "common/bits.h"
#include "ppu/layer.h"

class Window : public Register<Window, 1>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 1);

        data[index] = byte;

        flags   = bits<0, 5>(byte) | LF_BDP;
        effects = bits<5, 1>(byte);
    }

    uint flags   = 0;
    uint effects = 0;
};

class WindowIn : public Register<WindowIn, 2>
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        if (index == 0)
            return win0.read<0>();
        else
            return win1.read<0>();
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            win0.write<0>(byte);
        else
            win1.write<0>(byte);
    }

    Window win0;
    Window win1;
};

class WindowOut : public Register<WindowOut, 2>
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        if (index == 0)
            return winout.read<0>();
        else
            return winobj.read<0>();
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            winout.write<0>(byte);
        else
            winobj.write<0>(byte);
    }

    Window winout;
    Window winobj;
};
