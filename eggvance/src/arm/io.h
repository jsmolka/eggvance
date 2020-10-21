#pragma once

#include <functional>

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
    WaitControl()
    {
        updateCycles();
    }

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
        updateCycles();
    }

    int cyclesHalf(u32 addr, bool sequential) const
    {
        return cycles_half[sequential][addr >> 24];
    }

    int cyclesWord(u32 addr, bool sequential) const
    {
        return cycles_word[sequential][addr >> 24];
    }

    uint sram     = 0;
    uint ws0_n    = 0;
    uint ws0_s    = 0;
    uint ws1_n    = 0;
    uint ws1_s    = 0;
    uint ws2_n    = 0;
    uint ws2_s    = 0;
    uint prefetch = 0;

private:
    void updateCycles()
    {
        static constexpr int kNonSeq[4] = { 4, 3, 2, 8 };
        static constexpr int kWs0Seq[2] = { 2, 1 };
        static constexpr int kWs1Seq[2] = { 4, 1 };
        static constexpr int kWs2Seq[2] = { 8, 1 };

        cycles_half[0][0x8] = cycles_half[0][0x9] = kNonSeq[ws0_n];
        cycles_half[1][0x8] = cycles_half[1][0x9] = kWs0Seq[ws0_s];
        cycles_half[0][0xA] = cycles_half[0][0xB] = kNonSeq[ws1_n];
        cycles_half[1][0xA] = cycles_half[1][0xB] = kWs1Seq[ws1_s];
        cycles_half[0][0xC] = cycles_half[0][0xD] = kNonSeq[ws2_n];
        cycles_half[1][0xC] = cycles_half[1][0xD] = kWs2Seq[ws2_s];

        cycles_word[0][0x8] = cycles_word[0][0x9] = 2 * kNonSeq[ws0_n];
        cycles_word[1][0x8] = cycles_word[1][0x9] = 2 * kWs0Seq[ws0_s];
        cycles_word[0][0xA] = cycles_word[0][0xB] = 2 * kNonSeq[ws1_n];
        cycles_word[1][0xA] = cycles_word[1][0xB] = 2 * kWs1Seq[ws1_s];
        cycles_word[0][0xC] = cycles_word[0][0xD] = 2 * kNonSeq[ws2_n];
        cycles_word[1][0xC] = cycles_word[1][0xD] = 2 * kWs2Seq[ws2_s];

        cycles_half[0][0xE] = cycles_half[0][0xF] = kNonSeq[sram];
        cycles_half[1][0xE] = cycles_half[1][0xF] = kNonSeq[sram];
        cycles_word[0][0xE] = cycles_word[0][0xF] = kNonSeq[sram];
        cycles_word[1][0xE] = cycles_word[1][0xF] = kNonSeq[sram];
    }

    int cycles_half[2][256] =
    {
        { 1, 1, 3, 1, 1, 1, 1, 1 },
        { 1, 1, 3, 1, 1, 1, 1, 1 }
    };

    int cycles_word[2][256] =
    {
        { 1, 1, 6, 1, 1, 2, 2, 1 },
        { 1, 1, 6, 1, 1, 2, 2, 1 }
    };
};

class IrqMaster : public Register<u32, 0x0001>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        process();
    }

    std::function<void(void)> process;
};

class IrqEnable : public Register<u16, 0x3FFF>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        process();
    }

    std::function<void(void)> process;
};

class IrqRequest : public Register<u16, 0x3FFF>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        static_assert(Index < kSize);

        data[Index] &= ~(byte & bit::byte<Index>(kMask));

        process();
    }

    std::function<void(void)> process;
};

class ArmIo
{
public:
    friend class DmaChannel;
    friend class Io;

protected:
    HaltControl halt_control;
    WaitControl wait_control;

    struct
    {
        IrqMaster  master;
        IrqEnable  enable;
        IrqRequest request;
    } irq;
};
