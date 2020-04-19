#pragma once

#include "common/bits.h"
#include "common/register.h"
#include "ppu/layer.h"

struct Window
{
    inline void write(u8 byte)
    {
        flags = bits::seq<0, 5>(byte) | LF_BDP;
        blend = bits::seq<5, 1>(byte);
    }

    uint flags = 0;
    uint blend = 0;
};

class WindowInside : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
            win0.write(byte);
        else
            win1.write(byte);
    }

    Window win0;
    Window win1;
};

class WindowOutside : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
            winout.write(byte);
        else
            winobj.write(byte);
    }

    Window winout;
    Window winobj;
};
