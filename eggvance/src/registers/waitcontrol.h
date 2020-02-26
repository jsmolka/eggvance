#pragma once

#include "register.h"
#include "common/bits.h"

class WaitControl : public Register<WaitControl, 2>
{
public:
    WaitControl()
    {
        updateCycles();
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            sram  = bits<0, 2>(byte);
            ws0_n = bits<2, 2>(byte);
            ws0_s = bits<4, 1>(byte);
            ws1_n = bits<5, 2>(byte);
            ws1_s = bits<7, 1>(byte);
        }
        else
        {
            ws2_n    = bits<0, 2>(byte);
            ws2_s    = bits<2, 1>(byte);
            prefetch = bits<6, 1>(byte);
        }
        updateCycles();
    }

    inline uint cyclesHalf(u32 addr, bool sequential) const
    {
        return cycles_half[static_cast<uint>(sequential)][addr >> 24];
    }

    inline uint cyclesWord(u32 addr, bool sequential) const
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
    inline void updateCycles()
    {
        // Todo: +1

        static constexpr uint nonseq[4] = { 4, 3, 2, 8 };
        static constexpr uint ws0seq[4] = { 2, 1 };
        static constexpr uint ws1seq[4] = { 4, 1 };
        static constexpr uint ws2seq[4] = { 8, 1 };

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

    uint cycles_half[2][256]
    {
        { 1, 1, 3, 1, 1, 1, 1, 1 },
        { 1, 1, 3, 1, 1, 1, 1, 1 }
    };

    uint cycles_word[2][256]
    {
        { 1, 1, 6, 1, 1, 2, 2, 1 },
        { 1, 1, 6, 1, 1, 2, 2, 1 }
    };
};
