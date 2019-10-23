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

    int sram;
    WaitState ws0;
    WaitState ws1;
    WaitState ws2;
    int phi;
    int prefetch;

    int cycles16[2][256] 
    {
        { 1, 1, 3, 1, 1, 1, 1, 1 },
        { 1, 1, 3, 1, 1, 1, 1, 1 }
    };
    int cycles32[2][256]
    {
        { 1, 1, 6, 1, 1, 2, 2, 1 },
        { 1, 1, 6, 1, 1, 2, 2, 1 }
    };

private:
    void update();
};

#include "waitcnt.inl"
