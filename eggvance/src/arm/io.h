#pragma once

#include <functional>

#include "constants.h"
#include "base/register.h"

class HaltControl : public RegisterW<u8>
{
public:
    template<uint Index>
    void write(u8 byte);
};

class WaitControl : public Register<u16>
{
public:
    WaitControl();

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            sram  = bit::seq<0, 2>(byte);
            ws0_n = bit::seq<2, 2>(byte);
            ws0_s = bit::seq<4, 1>(byte);
            ws1_n = bit::seq<5, 2>(byte);
            ws1_s = bit::seq<7, 1>(byte);
        }
        else
        {
            ws2_n    = bit::seq<0, 2>(byte);
            ws2_s    = bit::seq<2, 1>(byte);
            prefetch = bit::seq<6, 1>(byte);
        }
        update();
    }

    int cyclesHalf(u32 addr, Access access) const { return cycles_half[(addr >> 25) & 0x3][static_cast<uint>(access)]; }
    int cyclesWord(u32 addr, Access access) const { return cycles_word[(addr >> 25) & 0x3][static_cast<uint>(access)]; }

    uint sram     = 0;
    uint ws0_n    = 0;
    uint ws0_s    = 0;
    uint ws1_n    = 0;
    uint ws1_s    = 0;
    uint ws2_n    = 0;
    uint ws2_s    = 0;
    uint prefetch = 0;

private:
    void update();

    int cycles_half[4][2];
    int cycles_word[4][2];
};

class IrqMaster : public Register<u32, 0x0001>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        on_write();
    }

    std::function<void(void)> on_write;
};

class IrqEnable : public Register<u16, 0x3FFF>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        on_write();
    }

    std::function<void(void)> on_write;
};

class IrqRequest : public Register<u16, 0x3FFF>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        static_assert(Index < kSize);

        data[Index] &= ~(byte & bit::byte<Index>(kMask));

        on_write();
    }

    std::function<void(void)> on_write;
};

