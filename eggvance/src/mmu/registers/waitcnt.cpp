#include "waitcnt.h"

#include "ppu/ppu.h"

void WaitCnt::WaitState::reset()
{
    *this = {};
}

void WaitCnt::reset()
{
    sram     = 0;
    prefetch = 0;
    phi      = 0;
    ws0.reset();
    ws1.reset();
    ws2.reset();

    update();
}

int WaitCnt::cyclesHalf(u32 addr, int sequential) const
{
    return cycles_half[sequential][addr >> 24];
}

int WaitCnt::cyclesWord(u32 addr, int sequential) const
{
    return cycles_word[sequential][addr >> 24];
}

void WaitCnt::update()
{
    static constexpr int nonseq[4] = { 4, 3, 2, 8 };
    static constexpr int ws0seq[4] = { 2, 1 };
    static constexpr int ws1seq[4] = { 4, 1 };
    static constexpr int ws2seq[4] = { 8, 1 };

    cycles_half[0][0x8] = cycles_half[0][0x9] = nonseq[ws0.n] + 1;
    cycles_half[1][0x8] = cycles_half[1][0x9] = ws0seq[ws0.s] + 1;
    cycles_half[0][0xA] = cycles_half[0][0xB] = nonseq[ws1.n] + 1;
    cycles_half[1][0xA] = cycles_half[1][0xB] = ws1seq[ws1.s] + 1;
    cycles_half[0][0xC] = cycles_half[0][0xD] = nonseq[ws2.n] + 1;
    cycles_half[1][0xC] = cycles_half[1][0xD] = ws2seq[ws2.s] + 1;

    cycles_word[0][0x8] = cycles_word[0][0x9] = 2 * nonseq[ws0.n] + 1;
    cycles_word[1][0x8] = cycles_word[1][0x9] = 2 * ws0seq[ws0.s] + 1;
    cycles_word[0][0xA] = cycles_word[0][0xB] = 2 * nonseq[ws1.n] + 1;
    cycles_word[1][0xA] = cycles_word[1][0xB] = 2 * ws1seq[ws1.s] + 1;
    cycles_word[0][0xC] = cycles_word[0][0xD] = 2 * nonseq[ws2.n] + 1;
    cycles_word[1][0xC] = cycles_word[1][0xD] = 2 * ws2seq[ws2.s] + 1;

    cycles_half[0][0xE] = nonseq[sram] + 1;
    cycles_half[1][0xE] = nonseq[sram] + 1;
    cycles_word[0][0xE] = nonseq[sram] + 1;
    cycles_word[1][0xE] = nonseq[sram] + 1;
}
