#include "waitcnt.h"

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

void WaitCnt::update()
{
    static constexpr int nonseq[4] = { 4, 3, 2, 8 };
    static constexpr int ws0seq[4] = { 2, 1 };
    static constexpr int ws1seq[4] = { 4, 1 };
    static constexpr int ws2seq[4] = { 8, 1 };

    cycles16[0][0x8] = cycles16[0][0x9] = nonseq[ws0.n];
    cycles16[1][0x8] = cycles16[1][0x9] = ws0seq[ws0.s];
    cycles16[0][0xA] = cycles16[0][0xB] = nonseq[ws1.n];
    cycles16[1][0xA] = cycles16[1][0xB] = ws1seq[ws1.s];
    cycles16[0][0xC] = cycles16[0][0xD] = nonseq[ws2.n];
    cycles16[1][0xC] = cycles16[1][0xD] = ws2seq[ws2.s];

    cycles32[0][0x8] = cycles32[0][0x9] = 2 * nonseq[ws0.n];
    cycles32[1][0x8] = cycles32[1][0x9] = 2 * ws0seq[ws0.s];
    cycles32[0][0xA] = cycles32[0][0xB] = 2 * nonseq[ws1.n];
    cycles32[1][0xA] = cycles32[1][0xB] = 2 * ws1seq[ws1.s];
    cycles32[0][0xC] = cycles32[0][0xD] = 2 * nonseq[ws2.n];
    cycles32[1][0xC] = cycles32[1][0xD] = 2 * ws2seq[ws2.s];

    cycles16[0][0xE] = nonseq[sram];
    cycles16[1][0xE] = nonseq[sram];
    cycles32[0][0xE] = nonseq[sram];
    cycles32[1][0xE] = nonseq[sram];
}
