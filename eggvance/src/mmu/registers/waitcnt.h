#pragma once

#include "common/macros.h"
#include "common/utility.h"

class WaitCnt
{
public:
    struct WaitState
    {
        void reset();

        int n;
        int s;
    };

    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int cyclesHalf(u32 addr, int sequential) const;
    int cyclesWord(u32 addr, int sequential) const;

    int sram;
    WaitState ws0;
    WaitState ws1;
    WaitState ws2;
    int phi;
    int prefetch;

private:
    void update();

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

#include "waitcnt.inl"
