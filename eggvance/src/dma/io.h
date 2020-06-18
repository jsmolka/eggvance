#pragma once

#include "base/bits.h"
#include "base/register.h"

struct DmaIo
{
    XRegisterW<4> sad;
    XRegisterW<4> dad;

    class DmaCount : public XRegisterW<2>
    {
    public:
        uint count(uint id) const
        {
            if (value == 0)
                return id < 3 ? 0x4000 : 0x1'0000;
            else
                return value;
        }
    } count;

    class DmaControl : public RegisterRW<2>
    {
    public:
        template<uint index>
        inline u8 read() const
        {
            static_assert(index < 2);

            if (index == 0)
                return data[0];
            else
                return (data[1] & ~(1 << 7)) | (enable << 7);
        }

        template<uint index>
        inline void write(u8 byte)
        {
            static_assert(index < 2);

            data[index] = byte;

            if (index == 0)
            {
                dadcnt = bits::seq<5, 2>(byte);
                sadcnt = bits::seq<7, 1>(byte) << 0 | (sadcnt & ~0x1);
            }
            else
            {
                sadcnt = bits::seq<0, 1>(byte) << 1 | (sadcnt & ~0x2);
                repeat = bits::seq<1, 1>(byte);
                word   = bits::seq<2, 1>(byte);
                timing = bits::seq<4, 2>(byte);
                irq    = bits::seq<6, 1>(byte);
                reload = bits::seq<7, 1>(byte) ^ enable;
                enable = bits::seq<7, 1>(byte);
            }
        }

        uint sadcnt = 0;
        uint dadcnt = 0;
        uint repeat = 0;
        uint word   = 0;
        uint timing = 0;
        uint irq    = 0;
        uint enable = 0;
        bool reload = false;
    } control;
};
