#pragma once

#include "base/bits.h"
#include "base/register.h"

class WaitControl : public RegisterRW<2>
{
public:
    WaitControl()
    {
        updateCycles();
    }

    template<uint index>
    void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            sram  = bits::seq<0, 2>(byte);
            ws0_n = bits::seq<2, 2>(byte);
            ws0_s = bits::seq<4, 1>(byte);
            ws1_n = bits::seq<5, 2>(byte);
            ws1_s = bits::seq<7, 1>(byte);
        }
        else
        {
            ws2_n    = bits::seq<0, 2>(byte);
            ws2_s    = bits::seq<2, 1>(byte);
            prefetch = bits::seq<6, 1>(byte);
        }
        updateCycles();
    }

    int cyclesHalf(u32 addr, bool sequential) const
    {
        return cycles_half[static_cast<uint>(sequential)][addr >> 24];
    }

    int cyclesWord(u32 addr, bool sequential) const
    {
        return cycles_word[static_cast<uint>(sequential)][addr >> 24];
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
        static constexpr int nonseq[4] = { 4, 3, 2, 8 };
        static constexpr int ws0seq[2] = { 2, 1 };
        static constexpr int ws1seq[2] = { 4, 1 };
        static constexpr int ws2seq[2] = { 8, 1 };

        cycles_half[0][0x8] = cycles_half[0][0x9] = nonseq[ws0_n];
        cycles_half[1][0x8] = cycles_half[1][0x9] = ws0seq[ws0_s];
        cycles_half[0][0xA] = cycles_half[0][0xB] = nonseq[ws1_n];
        cycles_half[1][0xA] = cycles_half[1][0xB] = ws1seq[ws1_s];
        cycles_half[0][0xC] = cycles_half[0][0xD] = nonseq[ws2_n];
        cycles_half[1][0xC] = cycles_half[1][0xD] = ws2seq[ws2_s];

        cycles_word[0][0x8] = cycles_word[0][0x9] = 2 * nonseq[ws0_n];
        cycles_word[1][0x8] = cycles_word[1][0x9] = 2 * ws0seq[ws0_s];
        cycles_word[0][0xA] = cycles_word[0][0xB] = 2 * nonseq[ws1_n];
        cycles_word[1][0xA] = cycles_word[1][0xB] = 2 * ws1seq[ws1_s];
        cycles_word[0][0xC] = cycles_word[0][0xD] = 2 * nonseq[ws2_n];
        cycles_word[1][0xC] = cycles_word[1][0xD] = 2 * ws2seq[ws2_s];

        cycles_half[0][0xE] = cycles_half[0][0xF] = nonseq[sram];
        cycles_half[1][0xE] = cycles_half[1][0xF] = nonseq[sram];
        cycles_word[0][0xE] = cycles_word[0][0xF] = nonseq[sram];
        cycles_word[1][0xE] = cycles_word[1][0xF] = nonseq[sram];
    }

    int cycles_half[2][256]
    {
        { 1, 1, 3, 1, 1, 1, 1, 1 },
        { 1, 1, 3, 1, 1, 1, 1, 1 }
    };

    int cycles_word[2][256]
    {
        { 1, 1, 6, 1, 1, 2, 2, 1 },
        { 1, 1, 6, 1, 1, 2, 2, 1 }
    };
};
