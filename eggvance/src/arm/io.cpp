#include "io.h"

#include "arm.h"
#include "constants.h"

template<uint Index>
void HaltControl::write(u8 byte)
{
    RegisterW::write<Index>(byte);

    arm.state |= kStateHalt;
}

template void HaltControl::write<0>(u8);

WaitControl::WaitControl()
{
    update();
}

void WaitControl::update()
{
    static constexpr int kNonSeq[4] = { 4, 3, 2, 8 };
    static constexpr int kWs0Seq[2] = { 2, 1 };
    static constexpr int kWs1Seq[2] = { 4, 1 };
    static constexpr int kWs2Seq[2] = { 8, 1 };

    for (uint x = 0; x < 2; ++x)
    {
        cycles_half[0x8 + x][0] = kNonSeq[ws0_n] + 1;
        cycles_half[0xA + x][0] = kNonSeq[ws1_n] + 1;
        cycles_half[0xC + x][0] = kNonSeq[ws2_n] + 1;
        
        cycles_half[0x8 + x][1] = kWs0Seq[ws0_s] + 1;
        cycles_half[0xA + x][1] = kWs1Seq[ws1_s] + 1;
        cycles_half[0xC + x][1] = kWs2Seq[ws2_s] + 1;

        cycles_word[0x8 + x][0] = cycles_half[0x8][0] + cycles_half[0x8][1];
        cycles_word[0XA + x][0] = cycles_half[0XA][0] + cycles_half[0XA][1];
        cycles_word[0xC + x][0] = cycles_half[0xC][0] + cycles_half[0xC][1];
        
        cycles_word[0x8 + x][1] = 2 * cycles_half[0x8][1];
        cycles_word[0XA + x][1] = 2 * cycles_half[0XA][1];
        cycles_word[0xC + x][1] = 2 * cycles_half[0xC][1];

        cycles_half[0xE + x][x] = kNonSeq[sram] + 1;
        cycles_word[0xE + x][x] = kNonSeq[sram] + 1;
    }
}
