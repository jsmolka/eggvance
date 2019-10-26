#pragma once

#include "register.h"

class WaitControl : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int cyclesHalf(u32 addr, int sequential) const;
    int cyclesWord(u32 addr, int sequential) const;

    int sram;
    int ws0_n;
    int ws0_s;
    int ws1_n;
    int ws1_s;
    int ws2_n;
    int ws2_s;
    int prefetch;

private:
    void updateCycles();

    int cycles_half[2][256]
    {
        { 1, 1, 3, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 3, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }
    };
    int cycles_word[2][256]
    {
        { 1, 1, 6, 1, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 6, 1, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0 }
    };
};
