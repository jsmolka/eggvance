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

    class DmaControl : public XRegister<2>
    {
    public:
        static constexpr uint kEnable = 1 << 7;

        template<uint Index>
        u8 read() const
        {
            static_assert(Index < 2);

            if (Index == 0)
                return data[0];
            else
                return (data[1] & ~(1 << 7)) | (enable << 7);
        }

        template<uint Index, uint Mask>
        void write(u8 byte)
        {
            data[Index] = byte & mask<Index, Mask>();

            if (Index == 0)
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
