#include "waitcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void WaitControl::reset()
{
    int sram     = 0;
    int ws0_n    = 0;
    int ws0_s    = 0;
    int ws1_n    = 0;
    int ws1_s    = 0;
    int ws2_n    = 0;
    int ws2_s    = 0;
    int prefetch = 0;

    updateCycles();
}

u8 WaitControl::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    return bytes[index];
}

void WaitControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    if (index == 0)
    {
        sram  = bits<0, 2>(byte);
        ws0_n = bits<2, 2>(byte);
        ws0_s = bits<4, 1>(byte);
        ws1_n = bits<5, 2>(byte);
        ws1_s = bits<7, 1>(byte);

        updateCycles();
    }
    else if (index == 1)
    {
        ws2_n    = bits<0, 2>(byte);
        ws2_s    = bits<2, 1>(byte);
        prefetch = bits<6, 1>(byte);

        updateCycles();
    }
    bytes[index] = byte;
}

int WaitControl::cyclesHalf(u32 addr, int sequential) const
{
    // Todo: Check if SRAM
    // Todo: Cycles for unused memory > 0?
    return cycles_half[sequential][addr >> 24];
}

int WaitControl::cyclesWord(u32 addr, int sequential) const
{
    // Todo: Check if SRAM
    // Todo: Cycles for unused memory > 0?
    return cycles_word[sequential][addr >> 24];
}

void WaitControl::updateCycles()
{
    static constexpr int nonseq[4] = { 4, 3, 2, 8 };
    static constexpr int ws0seq[4] = { 2, 1 };
    static constexpr int ws1seq[4] = { 4, 1 };
    static constexpr int ws2seq[4] = { 8, 1 };

    cycles_half[0][0x8] = cycles_half[0][0x9] = nonseq[ws0_n] + 1;
    cycles_half[1][0x8] = cycles_half[1][0x9] = ws0seq[ws0_s] + 1;
    cycles_half[0][0xA] = cycles_half[0][0xB] = nonseq[ws1_n] + 1;
    cycles_half[1][0xA] = cycles_half[1][0xB] = ws1seq[ws1_s] + 1;
    cycles_half[0][0xC] = cycles_half[0][0xD] = nonseq[ws2_n] + 1;
    cycles_half[1][0xC] = cycles_half[1][0xD] = ws2seq[ws2_s] + 1;

    cycles_word[0][0x8] = cycles_word[0][0x9] = 2 * nonseq[ws0_n] + 1;
    cycles_word[1][0x8] = cycles_word[1][0x9] = 2 * ws0seq[ws0_s] + 1;
    cycles_word[0][0xA] = cycles_word[0][0xB] = 2 * nonseq[ws1_n] + 1;
    cycles_word[1][0xA] = cycles_word[1][0xB] = 2 * ws1seq[ws1_s] + 1;
    cycles_word[0][0xC] = cycles_word[0][0xD] = 2 * nonseq[ws2_n] + 1;
    cycles_word[1][0xC] = cycles_word[1][0xD] = 2 * ws2seq[ws2_s] + 1;

    cycles_half[0][0xE] = nonseq[sram] + 1;
    cycles_half[1][0xE] = nonseq[sram] + 1;
    cycles_word[0][0xE] = nonseq[sram] + 1;
    cycles_word[1][0xE] = nonseq[sram] + 1;
}
